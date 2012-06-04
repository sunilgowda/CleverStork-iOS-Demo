//
//  MainViewController.h
//  CleverStorkDemo
//
//  Created by Sunil Gowda on 4/21/12.
//  Copyright (c) 2012 CleverStork. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CleverStork/CleverStork.h>

@interface MainViewController : UIViewController<UITableViewDataSource, CleverStorkDelegate> {
    UIButton *startButton;
    UISwitch *autoRepeat;
    UITableView *appInfoTable;
    NSArray *keys;
    NSArray *values;
    int numChecks;
    float avgCheckTime;
}

@property (nonatomic, retain) IBOutlet UIButton *startButton;
@property (nonatomic, retain) IBOutlet UISwitch *autoRepeat;
@property (nonatomic, retain) IBOutlet UITableView *appInfoTable;

-(IBAction) autoRepeatValueChanged:(id)sender;
-(IBAction) startButtonTapped:(id)sender;

@end
