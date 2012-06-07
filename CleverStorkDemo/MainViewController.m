//
//  MainViewController.m
//  CleverStorkDemo
//
//  Created by Sunil Gowda on 4/21/12.
//  Copyright (c) 2012 CleverStork. All rights reserved.
//

#import "MainViewController.h"

@implementation MainViewController

@synthesize autoRepeat, startButton, appInfoTable;

-(id) init {
    if( self = [super initWithNibName:@"MainViewController" bundle:nil] ) {
        keys = [[NSArray arrayWithObjects:@"Status", @"Current Version", @"Latest Version", @"Pre-Update Message", @"Post-Update Message", @"Minimum Version", @"Update Required Message", @"Time", nil] retain];
    }
    return self;
}

-(void) dealloc {
    [autoRepeat release];
    [startButton release];
    [appInfoTable release];
    [super dealloc];
}

-(void) viewDidLoad {
    [super viewDidLoad];
    appInfoTable.dataSource = self;
}

#pragma mark -

#pragma mark Private Methods

-(NSString *) storkStatusString:(CleverStorkStatus)status {
    switch(status) {
        case csCurrent: return @"Current";
        case csPending: return @"Pending";
        case csUpdateAvailable: return @"Update Available";
        case csUpdateRequired: return @"Update Required";
    }
    return nil;
}

#pragma mark -

#pragma mark IBActions

-(IBAction) autoRepeatValueChanged:(id)sender {
    if(autoRepeat.on) {
        [startButton setTitle:@"Start Benchmark" forState:UIControlStateNormal];
        [startButton setTitle:@"Stop Benchmark" forState:UIControlStateSelected];
    } else {
        [startButton setTitle:@"Check version" forState:UIControlStateNormal];
    }
}

// Demo of using the condition to wait for completion of the version check
-(IBAction) startButtonTapped:(id)sender {
    
    NSDate *start = [NSDate date];
    startButton.enabled = NO;
    
    if([sender isKindOfClass:[UIButton class]] && startButton.selected) {
        // user tapped "stop benchmark" - 
        autoRepeat.on = NO;
        startButton.selected = NO;
        [startButton setTitle:@"Check version" forState:UIControlStateNormal];
    } else if(autoRepeat.on) {
        if(!startButton.selected) {
            avgCheckTime = 0.0;
            numChecks = 0;
        }
        startButton.selected = YES;
    }
    
    CleverStork *stork = [[CleverStork alloc] initWithKey:@"0b8ce971fd6d5b282051" token:@"e0b1e88809df284c7513"];
    [stork doCheckWithDelegate:self];
    
    
    /// Typically this code would be in your app delegate's applicationDidBecomeActive
    /// You can continue initialization while the check is made
    
    /// and then block to check the status
    [stork.completion lock];
    while (!stork.completed) [stork.completion wait];
    [stork.completion unlock];
    
    NSDate *end = [NSDate date];
    NSTimeInterval duration = [end timeIntervalSinceDate:start];
    
    avgCheckTime = (avgCheckTime * numChecks + duration) / ++numChecks;
    
    
    if(autoRepeat.on) {
        [NSTimer scheduledTimerWithTimeInterval:1.0f target:self selector:@selector(startButtonTapped:) userInfo:nil repeats:NO];     
    } else {
        if(stork.status == csUpdateRequired || stork.status == csUpdateAvailable) {
            [stork presentUpdateOptionsWithDelegate:nil];    
        }
    }
    
    // release previous values if we are in benchmark (auto-repeat) mode
    [values release];
    values = [NSArray arrayWithObjects:
              [self storkStatusString:stork.status], 
              [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"], 
              stork.latestVersion != nil? stork.latestVersion : @"", 
              stork.preUpdateMessage != nil? stork.preUpdateMessage : @"", 
              stork.postUpdateMessage != nil? stork.postUpdateMessage : @"",
              stork.minimumVersion != nil ? stork.minimumVersion : @"", 
              stork.updateRequiredMessage != nil? stork.updateRequiredMessage : @"", 
              [NSString stringWithFormat:@"%.0f milliseconds (avg=%.1fms over %d req)", duration * 1000.0f, avgCheckTime * 1000.0f, numChecks], 
              nil];
    [values retain];
    
    [appInfoTable reloadData];
    startButton.enabled = YES;
    [stork release];
}

#pragma mark -
        
#pragma mark CleverStorkDelegate methods

-(void) cleverStork:(CleverStork *)stork didCompleteCheck:(CleverStorkStatus)status {

    // You can also consume the version check results here.
    // And use [stork presentUpdateOptionsWithDelegate:] to present an alert 
    // or display the messages in your custom UI and invoke doUpdate if the user chooses to update
    
}

-(void) cleverStork:(CleverStork *)stork didAcceptUpdateOption:(_Bool)doUpdate {
    
}

#pragma mark -

#pragma mark UITableViewDataSource methods

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [values count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *cellIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
    
    if(cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:cellIdentifier];
    }
    NSString *title = [keys objectAtIndex:indexPath.row];
    NSString *value = [values objectAtIndex:indexPath.row];
    
    cell.textLabel.text = title;
    cell.detailTextLabel.text = value;
    
    return cell;
}

#pragma mark -

@end
