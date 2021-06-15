//
//  NEMainTableViewController.m
//  NECollectionViewLayout_Example
//
//  Created by Daniel on 2019/12/2.
//  Copyright © 2019 Daniel. All rights reserved.
//

#import "NEMainTableViewController.h"

@interface NEMainTableViewController ()

@property (nonatomic, strong) NSArray *dataSource;

@end

@implementation NEMainTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self.tableView registerClass:UITableViewCell.class forCellReuseIdentifier:@"cell"];
    self.dataSource = @[
        @{
            @"title": @"update",
            @"class": @"NEUpdateCollectionViewController"
        },
        @{
            @"title": @"background",
            @"class": @"NEBackgroundCollectionViewController"
        },
        @{
            @"title": @"scroll horizontal",
            @"class": @"NEScrollDirectionCollectionViewController"
        },@{
            @"title": @"Alignment",
            @"class": @"NEAlignmentCollectionViewController"
        },
        @{
            @"title": @"pin",
            @"class": @"NEPinCollectionViewController"
        },
        @{
            @"title": @"insert",
            @"class": @"NEInsertCollectionViewController"
        },
        @{
            @"title": @"delete",
            @"class": @"NEDeleteCollectionViewController"
        },
        // TODO:
//        @{
//            @"title": @"reorder",
//            @"class": @"NEReorderCollectionViewController"
//        },
        @{
            @"title": @"Move",
            @"class": @"NEMoveCollectionViewController"
        },
    ];
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.dataSource.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"cell" forIndexPath:indexPath];
    cell.textLabel.text = self.dataSource[indexPath.row][@"title"];
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    Class cls = NSClassFromString(self.dataSource[indexPath.row][@"class"]);
    UIViewController *vc = [[cls alloc] init];
    [self.navigationController pushViewController:vc animated:YES];
}

@end
