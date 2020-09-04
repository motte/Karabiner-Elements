#import "DevicesTableViewController.h"
#import "DevicesTableCellView.h"
#import "FnFunctionKeysTableViewController.h"
#import "KarabinerKit/KarabinerKit.h"
#import "NotificationKeys.h"
#import "SimpleModificationsTableViewController.h"
#import <pqrs/weakify.h>

@interface DevicesTableViewController ()

@property(weak) IBOutlet NSTableView* tableView;
@property(weak) IBOutlet NSTableView* externalKeyboardTableView;
@property(weak) IBOutlet SimpleModificationsTableViewController* simpleModificationsTableViewController;
@property(weak) IBOutlet FnFunctionKeysTableViewController* fnFunctionKeysTableViewController;
@property(weak) IBOutlet NSPanel* hasCapsLockLedConfirmationPanel;
@property(weak) IBOutlet NSWindow* window;
@property KarabinerKitSmartObserverContainer* observers;

@end

@implementation DevicesTableViewController

- (void)setup {
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

                             [self.tableView reloadData];
                             [self.externalKeyboardTableView reloadData];
                           }];
    [self.observers addObserver:o notificationCenter:center];
  }

  {
    NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
    id o = [center addObserverForName:kKarabinerKitDevicesAreUpdated
                               object:nil
                                queue:[NSOperationQueue mainQueue]
                           usingBlock:^(NSNotification* note) {
                             @strongify(self);
                             if (!self) {
                               return;
                             }

                             [self.tableView reloadData];
                             [self.externalKeyboardTableView reloadData];
                           }];
    [self.observers addObserver:o notificationCenter:center];
  }
}

- (void)valueChanged:(id)sender {
  KarabinerKitCoreConfigurationModel* coreConfigurationModel = [KarabinerKitConfigurationManager sharedManager].coreConfigurationModel;

  NSInteger row = [self.tableView rowForView:sender];
  if (row != -1) {
    DevicesTableCellView* cellView = [self.tableView viewAtColumn:0 row:row makeIfNecessary:NO];
    libkrbn_device_identifiers deviceIdentifiers = cellView.deviceIdentifiers;
    [coreConfigurationModel setSelectedProfileDeviceIgnore:&(deviceIdentifiers)
                                                     value:(cellView.checkbox.state == NSControlStateValueOff)];
    [coreConfigurationModel save];
    goto finish;
  }

  row = [self.externalKeyboardTableView rowForView:sender];
  if (row != -1) {
    DevicesTableCellView* cellView = [self.externalKeyboardTableView viewAtColumn:0 row:row makeIfNecessary:NO];
    libkrbn_device_identifiers deviceIdentifiers = cellView.deviceIdentifiers;
    [coreConfigurationModel setSelectedProfileDeviceDisableBuiltInKeyboardIfExists:&(deviceIdentifiers)
                                                                             value:(cellView.checkbox.state == NSControlStateValueOn)];
    [coreConfigurationModel save];
    goto finish;
  }

finish:
  [self.simpleModificationsTableViewController updateConnectedDevicesMenu];
  [self.fnFunctionKeysTableViewController updateConnectedDevicesMenu];
}

- (void)hasCapsLockLedChanged:(id)sender {
  NSInteger row = [self.tableView rowForView:sender];
  if (row != -1) {
    KarabinerKitCoreConfigurationModel* coreConfigurationModel = [KarabinerKitConfigurationManager sharedManager].coreConfigurationModel;
    DevicesTableCellView* cellView = [self.tableView viewAtColumn:1 row:row makeIfNecessary:NO];
    libkrbn_device_identifiers deviceIdentifiers = cellView.deviceIdentifiers;

    if (cellView.checkbox.state == NSControlStateValueOff) {
      [coreConfigurationModel setSelectedProfileDeviceManipulateCapsLockLed:&(deviceIdentifiers)
                                                                      value:NO];
      [coreConfigurationModel save];

    } else {
      if (libkrbn_device_identifiers_is_apple(&deviceIdentifiers)) {
        [coreConfigurationModel setSelectedProfileDeviceManipulateCapsLockLed:&(deviceIdentifiers)
                                                                        value:YES];
        [coreConfigurationModel save];

      } else {
        [self.window beginSheet:self.hasCapsLockLedConfirmationPanel
              completionHandler:^(NSModalResponse returnCode) {
                if (returnCode == NSModalResponseOK) {
                  [coreConfigurationModel setSelectedProfileDeviceManipulateCapsLockLed:&(deviceIdentifiers)
                                                                                  value:YES];
                  [coreConfigurationModel save];

                } else {
                  cellView.checkbox.state = NSControlStateValueOff;
                }
              }];
      }
    }
  }
}

- (IBAction)setManipulateCapsLockLed:(id)sender {
  [self.window endSheet:self.hasCapsLockLedConfirmationPanel
             returnCode:NSModalResponseOK];
}

- (IBAction)cancelSetManipulateCapsLockLed:(id)sender {
  [self.window endSheet:self.hasCapsLockLedConfirmationPanel
             returnCode:NSModalResponseCancel];
}

@end
