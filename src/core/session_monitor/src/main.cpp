#include "constants.hpp"
#include "dispatcher_utility.hpp"
#include "karabiner_version.h"
#include "logger.hpp"
#include "process_utility.hpp"
#include "session_monitor/components_manager.hpp"
#include <iostream>

int main(int argc, const char* argv[]) {
  //
  // Initialize
  //

  auto scoped_dispatcher_manager = krbn::dispatcher_utility::initialize_dispatchers();

  signal(SIGUSR1, SIG_IGN);
  signal(SIGUSR2, SIG_IGN);

  //
  // Check euid
  //

  // - karabiner_session_monitor is invoked by user.
  // - karabiner_session_monitor is executed as root by setuid.
  if (geteuid() != 0) {
    std::cerr << "fatal: karabiner_session_monitor requires root privilege." << std::endl;
    return 1;
  }

  //
  // Setup logger
  //

  // We have to use `getuid` (not `geteuid`) since `karabiner_session_monitor` is run as root by suid.
  // (We have to make a log file which includes the real user ID in the file path.)
  krbn::logger::set_async_rotating_logger("session_monitor",
                                          fmt::format("/var/log/karabiner/session_monitor.{0}.log", getuid()),
                                          pqrs::spdlog::filesystem::log_directory_perms_0755);

  krbn::logger::get_logger()->info("version {0}", karabiner_version);

  //
  // Check another process
  //

  {
    // We have to use `getuid` (not `geteuid`) since `karabiner_session_monitor` is run as root by suid.
    // (We have to make pid file which includes the real user ID in the file path.)
    std::string pid_file_path = krbn::constants::get_pid_directory() +
                                fmt::format("/karabiner_session_monitor.{0}.pid", getuid());
    if (!krbn::process_utility::lock_single_application(pid_file_path)) {
      auto message = "Exit since another process is running.";
      krbn::logger::get_logger()->info(message);
      std::cerr << message << std::endl;
      return 1;
    }
  }

  //
  // Run components_manager
  //

  krbn::components_manager_killer::initialize_shared_components_manager_killer();

  // We have to use raw pointer (not smart pointer) to delete it in `dispatch_async`.
  krbn::session_monitor::components_manager* components_manager = nullptr;

  if (auto killer = krbn::components_manager_killer::get_shared_components_manager_killer()) {
    killer->kill_called.connect([&components_manager] {
      dispatch_async(dispatch_get_main_queue(), ^{
        {
          // Mark as main queue to avoid a deadlock in `pqrs::gcd::dispatch_sync_on_main_queue` in destructor.
          pqrs::gcd::scoped_running_on_main_queue_marker marker;

          delete components_manager;
        }

        CFRunLoopStop(CFRunLoopGetCurrent());
      });
    });
  }

  components_manager = new krbn::session_monitor::components_manager();
  components_manager->async_start();

  CFRunLoopRun();

  krbn::components_manager_killer::terminate_shared_components_manager_killer();

  krbn::logger::get_logger()->info("karabiner_session_monitor is terminated.");

  return 0;
}
