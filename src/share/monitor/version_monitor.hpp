#pragma once

// `krbn::version_monitor` can be used safely in a multi-threaded environment.

#include "constants.hpp"
#include "logger.hpp"
#include <fstream>
#include <nod/nod.hpp>
#include <pqrs/filesystem.hpp>
#include <pqrs/osx/file_monitor.hpp>
#include <pqrs/string.hpp>

namespace krbn {
class version_monitor final : public pqrs::dispatcher::extra::dispatcher_client {
public:
  // Signals (invoked from the shared dispatcher thread)

  nod::signal<void(const std::string& version)> changed;

  // Methods

  version_monitor(const version_monitor&) = delete;

  version_monitor(const std::string& version_file_path) : version_file_path_(version_file_path) {
    version_ = pqrs::osx::file_monitor::read_file(version_file_path_);

    std::vector<std::string> targets = {
        version_file_path_,
    };

    file_monitor_ = std::make_unique<pqrs::osx::file_monitor>(weak_dispatcher_,
                                                              targets);

    file_monitor_->file_changed.connect([this](auto&& changed_file_path,
                                               auto&& changed_file_body) {
      if (changed_file_body) {
        if (version_) {
          if (*version_ == *changed_file_body) {
            return;
          }
        } else if (!version_ && !changed_file_body) {
          return;
        }

        std::string version_string(std::begin(*changed_file_body),
                                   std::end(*changed_file_body));
        pqrs::string::trim(version_string);

        logger::get_logger()->info("Version is changed to {0}", version_string);

        version_ = changed_file_body;

        enqueue_to_dispatcher([this, version_string] {
          changed(version_string);
        });
      }
    });
  }

  virtual ~version_monitor(void) {
    detach_from_dispatcher([this] {
      file_monitor_ = nullptr;
    });
  }

  void async_start() {
    file_monitor_->async_start();
  }

  void async_manual_check(void) {
    file_monitor_->enqueue_file_changed(version_file_path_);
  }

private:
  std::string version_file_path_;

  std::shared_ptr<std::vector<uint8_t>> version_;
  std::unique_ptr<pqrs::osx::file_monitor> file_monitor_;
};
} // namespace krbn
