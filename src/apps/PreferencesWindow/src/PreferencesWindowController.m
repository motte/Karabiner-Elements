#import "PreferencesWindowController.h"
#import "ComplexModificationsParametersTabController.h"
#import "ComplexModificationsRulesTableViewController.h"
#import "DevicesTableViewController.h"
#import "FnFunctionKeysTableViewController.h"
#import "KarabinerKit/KarabinerKit.h"
#import "LogFileTextViewController.h"
#import "NotificationKeys.h"
#import "ProfilesTableViewController.h"
#import "SimpleModificationsMenuManager.h"
#import "SimpleModificationsTableViewController.h"
#import "SystemPreferencesManager.h"
#import "VirtualHIDKeyboardTypeBackgroundView.h"
#import "libkrbn/libkrbn.h"
#import <pqrs/weakify.h>

@interface PreferencesWindowController ()

@property(weak) IBOutlet ComplexModificationsParametersTabController* complexModificationsParametersTabController;
@property(weak) IBOutlet ComplexModificationsRulesTableViewController* complexModificationsRulesTableViewController;
@property(weak) IBOutlet DevicesTableViewController* devicesTableViewController;
@property(weak) IBOutlet FnFunctionKeysTableViewController* fnFunctionKeysTableViewController;
@property(weak) IBOutlet LogFileTextViewController* logFileTextViewController;
@property(weak) IBOutlet NSButton* useFkeysAsStandardFunctionKeysButton;
@property(weak) IBOutlet NSTableView* devicesTableView;
@property(weak) IBOutlet NSTextField* delayBeforeOpenDeviceText;
@property(weak) IBOutlet NSStepper* delayBeforeOpenDeviceStepper;
@property(weak) IBOutlet NSTableView* devicesExternalKeyboardTableView;
@property(weak) IBOutlet NSTableView* fnFunctionKeysTableView;
@property(weak) IBOutlet NSTableView* simpleModificationsTableView;
@property(weak) IBOutlet NSTextField* versionLabel;
@property(weak) IBOutlet NSTabViewItem* virtualHIDKeyboardTabViewItem;
@property(weak) IBOutlet NSTextField* virtualHIDKeyboardCountryCodeText;
@property(weak) IBOutlet NSStepper* virtualHIDKeyboardCountryCodeStepper;
@property(weak) IBOutlet NSTextField* virtualHIDKeyboardMouseKeyXYScaleText;
@property(weak) IBOutlet NSStepper* virtualHIDKeyboardMouseKeyXYScaleStepper;
@property(weak) IBOutlet NSButton* checkForUpdateOnStartupButton;
@property(weak) IBOutlet NSButton* systemDefaultProfileCopyButton;
@property(weak) IBOutlet NSTextField* systemDefaultProfileStateLabel;
@property(weak) IBOutlet NSButton* systemDefaultProfileRemoveButton;
@property(weak) IBOutlet NSButton* showInMenuBarButton;
@property(weak) IBOutlet NSButton* showProfileNameInMenuBarButton;
@property(weak) IBOutlet ProfilesTableViewController* profilesTableViewController;
@property(weak) IBOutlet SimpleModificationsMenuManager* simpleModificationsMenuManager;
@property(weak) IBOutlet SimpleModificationsTableViewController* simpleModificationsTableViewController;
@property(weak) IBOutlet SystemPreferencesManager* systemPreferencesManager;
@property KarabinerKitSmartObserverContainer* observers;

@end

@implementation PreferencesWindowController

- (void)setup {
  // ----------------------------------------
  // Setup

  [self.simpleModificationsMenuManager setup];
  [self.simpleModificationsTableViewController setup];
  [self.fnFunctionKeysTableViewController setup];
  [self.complexModificationsRulesTableViewController setup];
  [self.complexModificationsParametersTabController setup];
  [self.devicesTableViewController setup];
  [self setupDevicesParameters:nil];
  [self setupVirtualHIDKeyboardConfiguration:nil];
  [self.profilesTableViewController setup];
  [self setupMiscTabControls];
  [self.logFileTextViewController monitor];

  self.observers = [KarabinerKitSmartObserverContainer new];
  @weakify(self);

  {
    NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
    id o = [center addObserverForName:kKarabinerKitConfigurationIsLoaded
                               object:nil
                                queue:[NSOperationQueue mainQueue]
                           usingBlock:^(NSNotification* note) {
                             @strongify(self);
                             if (!self) {
                               return;
                             }

                             [self setupDevicesParameters:nil];
                             [self setupVirtualHIDKeyboardConfiguration:nil];
                             [self setupMiscTabControls];
                           }];
    [self.observers addObserver:o notificationCenter:center];
  }
  {
    NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
    id o = [center addObserverForName:kSystemPreferencesValuesAreUpdated
                               object:nil
                                queue:[NSOperationQueue mainQueue]
                           usingBlock:^(NSNotification* note) {
                             @strongify(self);
                             if (!self) {
                               return;
                             }

                             [self updateSystemPreferencesUIValues];
                           }];
    [self.observers addObserver:o notificationCenter:center];
  }

  // ----------------------------------------
  // Update UI values

  self.versionLabel.stringValue = [[NSBundle mainBundle] infoDictionary][@"CFBundleVersion"];

  [self.simpleModificationsTableView reloadData];
  [self.fnFunctionKeysTableView reloadData];
  [self.devicesTableView reloadData];
  [self.devicesExternalKeyboardTableView reloadData];

  [self updateSystemPreferencesUIValues];

  // ----------------------------------------
  libkrbn_launchctl_manage_session_monitor();
  libkrbn_launchctl_manage_observer_agent();
  libkrbn_launchctl_manage_grabber_agent();
  libkrbn_launchctl_manage_console_user_server(true);
}

- (void)show {
  [self.window makeKeyAndOrderFront:self];
  [NSApp activateIgnoringOtherApps:YES];
}

- (void)setupDevicesParameters:(id)sender {
  KarabinerKitCoreConfigurationModel* coreConfigurationModel = [KarabinerKitConfigurationManager sharedManager].coreConfigurationModel;
  NSInteger delayBeforeOpenDevice = coreConfigurationModel.selectedProfileParametersDelayMillisecondsBeforeOpenDevice;

  {
    NSTextField* t = self.delayBeforeOpenDeviceText;
    if (sender != t) {
      t.stringValue = @(delayBeforeOpenDevice).stringValue;
    }
  }
  {
    NSStepper* s = self.delayBeforeOpenDeviceStepper;
    if (sender != s) {
      s.integerValue = delayBeforeOpenDevice;
    }
  }
}

- (IBAction)changeDelayBeforeOpenDevice:(NSControl*)sender {
  // If sender.stringValue is empty, set "0"
  if (sender.integerValue == 0) {
    sender.integerValue = 0;
  }

  KarabinerKitCoreConfigurationModel* coreConfigurationModel = [KarabinerKitConfigurationManager sharedManager].coreConfigurationModel;
  coreConfigurationModel.selectedProfileParametersDelayMillisecondsBeforeOpenDevice = sender.integerValue;
  [coreConfigurationModel save];

  [self setupDevicesParameters:sender];
}

- (void)setupVirtualHIDKeyboardConfiguration:(id)sender {
  KarabinerKitCoreConfigurationModel* coreConfigurationModel = [KarabinerKitConfigurationManager sharedManager].coreConfigurationModel;
  NSInteger countryCode = coreConfigurationModel.selectedProfileVirtualHIDKeyboardCountryCode;
  NSInteger mouseKeyXYScale = coreConfigurationModel.selectedProfileVirtualHIDKeyboardMouseKeyXYScale;

  {
    NSTextField* t = self.virtualHIDKeyboardCountryCodeText;
    if (sender != t) {
      t.stringValue = @(countryCode).stringValue;
    }
  }
  {
    NSStepper* s = self.virtualHIDKeyboardCountryCodeStepper;
    if (sender != s) {
      s.integerValue = countryCode;
    }
  }
  {
    NSTextField* t = self.virtualHIDKeyboardMouseKeyXYScaleText;
    if (sender != t) {
      t.stringValue = @(mouseKeyXYScale).stringValue;
    }
  }
  {
    NSStepper* s = self.virtualHIDKeyboardMouseKeyXYScaleStepper;
    if (sender != s) {
      s.integerValue = mouseKeyXYScale;
    }
  }
}

- (IBAction)changeVirtualHIDKeyboardCountryCode:(NSControl*)sender {
  // If sender.stringValue is empty, set "0"
  if (sender.integerValue == 0) {
    sender.integerValue = 0;
  }

  KarabinerKitCoreConfigurationModel* coreConfigurationModel = [KarabinerKitConfigurationManager sharedManager].coreConfigurationModel;
  coreConfigurationModel.selectedProfileVirtualHIDKeyboardCountryCode = sender.integerValue;
  [coreConfigurationModel save];

  [self setupVirtualHIDKeyboardConfiguration:sender];
}

- (IBAction)changeVirtualHIDKeyboardMouseKeyXYScale:(NSControl*)sender {
  // If sender.stringValue is empty, set "0"
  if (sender.integerValue == 0) {
    sender.integerValue = 0;
  }

  KarabinerKitCoreConfigurationModel* coreConfigurationModel = [KarabinerKitConfigurationManager sharedManager].coreConfigurationModel;
  coreConfigurationModel.selectedProfileVirtualHIDKeyboardMouseKeyXYScale = sender.integerValue;
  [coreConfigurationModel save];

  [self setupVirtualHIDKeyboardConfiguration:sender];
}

- (void)updateSystemPreferencesUIValues {
  self.useFkeysAsStandardFunctionKeysButton.state = self.systemPreferencesManager.systemPreferencesModel.useFkeysAsStandardFunctionKeys ? NSControlStateValueOn : NSControlStateValueOff;
}

- (IBAction)updateSystemPreferencesValues:(id)sender {
  SystemPreferencesModel* model = self.systemPreferencesManager.systemPreferencesModel;

  if (sender == self.useFkeysAsStandardFunctionKeysButton) {
    model.useFkeysAsStandardFunctionKeys = (self.useFkeysAsStandardFunctionKeysButton.state == NSControlStateValueOn);
  }

  [self updateSystemPreferencesUIValues];
  [self.systemPreferencesManager updateSystemPreferencesValues:model];
}

- (void)setupMiscTabControls {
  KarabinerKitCoreConfigurationModel* coreConfigurationModel = [KarabinerKitConfigurationManager sharedManager].coreConfigurationModel;

  if (coreConfigurationModel.globalConfigurationCheckForUpdatesOnStartup) {
    self.checkForUpdateOnStartupButton.state = NSControlStateValueOn;
  } else {
    self.checkForUpdateOnStartupButton.state = NSControlStateValueOff;
  }

  if (libkrbn_system_core_configuration_file_path_exists()) {
    self.systemDefaultProfileStateLabel.hidden = YES;
    self.systemDefaultProfileRemoveButton.hidden = NO;
  } else {
    self.systemDefaultProfileStateLabel.hidden = NO;
    self.systemDefaultProfileRemoveButton.hidden = YES;
  }

  if (coreConfigurationModel.globalConfigurationShowInMenuBar) {
    self.showInMenuBarButton.state = NSControlStateValueOn;
  } else {
    self.showInMenuBarButton.state = NSControlStateValueOff;
  }

  if (coreConfigurationModel.globalConfigurationShowProfileNameInMenuBar) {
    self.showProfileNameInMenuBarButton.state = NSControlStateValueOn;
  } else {
    self.showProfileNameInMenuBarButton.state = NSControlStateValueOff;
  }
}

- (IBAction)changeMiscTabControls:(id)sender {
  KarabinerKitCoreConfigurationModel* coreConfigurationModel = [KarabinerKitConfigurationManager sharedManager].coreConfigurationModel;

  coreConfigurationModel.globalConfigurationCheckForUpdatesOnStartup = (self.checkForUpdateOnStartupButton.state == NSControlStateValueOn);
  coreConfigurationModel.globalConfigurationShowInMenuBar = (self.showInMenuBarButton.state == NSControlStateValueOn);
  coreConfigurationModel.globalConfigurationShowProfileNameInMenuBar = (self.showProfileNameInMenuBarButton.state == NSControlStateValueOn);

  [coreConfigurationModel save];

  libkrbn_launch_menu();
}

- (IBAction)checkForUpdatesStableOnly:(id)sender {
  libkrbn_check_for_updates_stable_only();
}

- (IBAction)checkForUpdatesWithBetaVersion:(id)sender {
  libkrbn_check_for_updates_with_beta_version();
}

- (IBAction)systemDefaultProfileCopy:(id)sender {
  // Ensure karabiner.json exists before copy.
  KarabinerKitCoreConfigurationModel* coreConfigurationModel = [KarabinerKitConfigurationManager sharedManager].coreConfigurationModel;
  [coreConfigurationModel save];

  NSString* path = @"/Library/Application Support/org.pqrs/Karabiner-Elements/scripts/copy_current_profile_to_system_default_profile.applescript";
  [[[NSAppleScript alloc] initWithContentsOfURL:[NSURL fileURLWithPath:path] error:nil] executeAndReturnError:nil];
  [self setupMiscTabControls];
}

- (IBAction)systemDefaultProfileRemove:(id)sender {
  NSString* path = @"/Library/Application Support/org.pqrs/Karabiner-Elements/scripts/remove_system_default_profile.applescript";
  [[[NSAppleScript alloc] initWithContentsOfURL:[NSURL fileURLWithPath:path] error:nil] executeAndReturnError:nil];
  [self setupMiscTabControls];
}

- (IBAction)launchUninstaller:(id)sender {
  NSString* path = @"/Library/Application Support/org.pqrs/Karabiner-Elements/scripts/uninstaller.applescript";
  [[[NSAppleScript alloc] initWithContentsOfURL:[NSURL fileURLWithPath:path] error:nil] executeAndReturnError:nil];
}

- (IBAction)launchMultitouchExtension:(id)sender {
  libkrbn_launch_multitouch_extension();
}

- (IBAction)openOfficialWebsite:(id)sender {
  [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"https://karabiner-elements.pqrs.org/"]];
}

- (IBAction)openGitHub:(id)sender {
  [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"https://github.com/pqrs-org/Karabiner-Elements"]];
}

- (IBAction)openConfigDirectory:(id)sender {
  NSURL* url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:libkrbn_get_user_configuration_directory()]
                          isDirectory:YES];
  NSLog(@"%@", url);
  [[NSWorkspace sharedWorkspace] openURL:url];
}

- (IBAction)restart:(id)sender {
  libkrbn_launchctl_restart_console_user_server();
  [KarabinerKit relaunch];
}

- (IBAction)quitWithConfirmation:(id)sender {
  [KarabinerKit quitKarabinerWithConfirmation];
}

@end
