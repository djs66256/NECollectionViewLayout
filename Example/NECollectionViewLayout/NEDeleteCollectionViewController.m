//
//  NEDeleteCollectionViewController.m
//  NECollectionViewLayout_Example
//
//  Created by Daniel on 2019/12/9.
//  Copyright © 2019 Daniel. All rights reserved.
//

#import <NECollectionViewLayout/NECollectionViewFlowLayout.h>
#import <NECollectionViewLayout/NEOptimizeCollectionView.h>
#import "NETextCollectionViewCell.h"
#import "NEHeaderCollectionReusableView.h"
#import "NEFooterCollectionReusableView.h"
#import "NEDeleteCollectionViewController.h"

@interface NEDeleteCollectionViewController () <NECollectionViewDelegateFlowLayout>

@property (nonatomic, strong) NSMutableArray<NSMutableArray<NSDictionary *> *> *dataSourece;

@end

@implementation NEDeleteCollectionViewController

- (instancetype)init
{
    NECollectionViewFlowLayout *layout = [NECollectionViewFlowLayout new];
    layout.pinToVisibleBoundsEnable = YES;
    layout.appearenceAnimator = [NECollectionViewFlowLayoutScaleAnimator new];
//    UICollectionViewFlowLayout *layout = [UICollectionViewFlowLayout new];
    layout.minimumLineSpacing = 10;
    layout.minimumInteritemSpacing = 10;
//    UICollectionViewLayout *layout = [NECollectionViewFlowLayout new];
    self = [self initWithCollectionViewLayout:layout];
    if (self) {
        
    }
    return self;
}

- (void)loadView {
    self.collectionView = [[NEOptimizeCollectionView alloc] initWithFrame:UIScreen.mainScreen.bounds
                                                     collectionViewLayout:self.collectionViewLayout];
    self.collectionView.backgroundColor = UIColor.blackColor;
    
//    self.collectionView = [[UICollectionView alloc] initWithFrame:UIScreen.mainScreen.bounds
//                                                     collectionViewLayout:self.collectionViewLayout];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    __auto_type deleteItem = [[UIBarButtonItem alloc] initWithTitle:@"item" style:UIBarButtonItemStylePlain target:self action:@selector(deleteItem)];
    __auto_type deletSection = [[UIBarButtonItem alloc] initWithTitle:@"section" style:UIBarButtonItemStylePlain target:self action:@selector(deletSection)];
    self.navigationItem.rightBarButtonItems = @[deletSection, deleteItem];
    
    // Register cell classes
    [self.collectionView registerClass:[NETextCollectionViewCell class] forCellWithReuseIdentifier:NSStringFromClass(NETextCollectionViewCell.class)];
    [self.collectionView registerClass:[NEHeaderCollectionReusableView class]
            forSupplementaryViewOfKind:UICollectionElementKindSectionHeader
                   withReuseIdentifier:NSStringFromClass(NEHeaderCollectionReusableView.class)];
    [self.collectionView registerClass:[NEFooterCollectionReusableView class]
            forSupplementaryViewOfKind:UICollectionElementKindSectionFooter
                   withReuseIdentifier:NSStringFromClass(NEFooterCollectionReusableView.class)];
    [self.collectionView registerClass:[UICollectionReusableView class]
            forSupplementaryViewOfKind:NECollectionElementKindSectionBackground
                   withReuseIdentifier:NSStringFromClass(UICollectionReusableView.class)];
    
    self.dataSourece = [NSMutableArray new];
    
    for (int i = 0; i < 10; i++) {
        NSMutableArray *section = [NSMutableArray new];
        for (int r = 0; r < 20; r++) {
            [section addObject:@{
                @"title" : [NSString stringWithFormat:@"%d-%d", i, r]
            }];
        }
        [self.dataSourece addObject:section];
    }
    
}

#pragma mark <UICollectionViewDataSource>

- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView {
    return self.dataSourece.count;
}


- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return [self.dataSourece[section] count];
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    NETextCollectionViewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:NSStringFromClass(NETextCollectionViewCell.class) forIndexPath:indexPath];
    cell.backgroundColor = [UIColor grayColor];
    cell.textLabel.text = self.dataSourece[indexPath.section][indexPath.item][@"title"];
    return cell;
}

- (UICollectionReusableView *)collectionView:(UICollectionView *)collectionView viewForSupplementaryElementOfKind:(NSString *)kind atIndexPath:(NSIndexPath *)indexPath {
    if ([kind isEqualToString:UICollectionElementKindSectionHeader]) {
        NEHeaderCollectionReusableView *view = [collectionView dequeueReusableSupplementaryViewOfKind:kind
                                                                                  withReuseIdentifier:NSStringFromClass(NEHeaderCollectionReusableView.class)
                                                                                         forIndexPath:indexPath];
        view.backgroundColor = [UIColor yellowColor];
        view.textLabel.text = [NSString stringWithFormat:@"Header - %@", @(indexPath.section).stringValue];
        return view;
    }
    else if ([kind isEqualToString:UICollectionElementKindSectionFooter]) {
        NEFooterCollectionReusableView *view = [collectionView dequeueReusableSupplementaryViewOfKind:kind
                                                                                  withReuseIdentifier:NSStringFromClass(NEFooterCollectionReusableView.class)
                                                                                         forIndexPath:indexPath];
        view.backgroundColor = [UIColor blueColor];
        view.textLabel.text = [NSString stringWithFormat:@"Footer - %@", @(indexPath.section).stringValue];
        return view;
    }
    else {
        UICollectionReusableView *view = [collectionView dequeueReusableSupplementaryViewOfKind:kind withReuseIdentifier:NSStringFromClass(UICollectionReusableView.class) forIndexPath:indexPath];
        view.backgroundColor = [UIColor colorWithWhite:1 alpha:0.3];
        view.clipsToBounds = YES;
        view.layer.cornerRadius = 10;
        return view;
    }
    return nil;
}

- (void)collectionView:(UICollectionView *)collectionView willDisplayCell:(UICollectionViewCell *)cell forItemAtIndexPath:(NSIndexPath *)indexPath {
    
}

- (void)collectionView:(UICollectionView *)collectionView didEndDisplayingCell:(UICollectionViewCell *)cell forItemAtIndexPath:(NSIndexPath *)indexPath {
    
}

- (UIEdgeInsets)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout insetForSectionAtIndex:(NSInteger)section {
    return UIEdgeInsetsMake(20, 20, 20, 20);
}

- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout referenceSizeForHeaderInSection:(NSInteger)section {
    return CGSizeMake(collectionView.frame.size.width, 60);
}

- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout referenceSizeForFooterInSection:(NSInteger)section {
    return CGSizeMake(collectionView.frame.size.width, 30);
}

- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath {
    return  CGSizeMake(collectionView.frame.size.width / 6, 88);
}

- (BOOL)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout backgroundVisibleForSectionAtIndex:(NSInteger)section {
    return YES;
}

- (NECollectionViewFlowLayoutAlignment)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout alignVerticalForSectionAtIndex:(NSInteger)section {
    return section % 3;
}

- (NECollectionViewFlowLayoutAlignment)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout alignHorizontalForSectionAtIndex:(NSInteger)section {
    return section % 4;
}

- (void)deleteItem {
    if (self.dataSourece.firstObject.count) {
        NSInteger index = self.dataSourece.firstObject.count - 1;
        [self.dataSourece.firstObject removeObjectAtIndex:index];
        [self.collectionView deleteItemsAtIndexPaths:@[[NSIndexPath indexPathForItem:index inSection:0]]];
    }
}

- (void)deletSection {
    if (self.dataSourece.count) {
        NSInteger i = 0; //self.dataSourece.count - 1;
        [self.dataSourece removeObjectAtIndex:i];
        [self.collectionView performBatchUpdates:^{
            [self.collectionView deleteSections:[NSIndexSet indexSetWithIndex:i]];
        } completion:^(BOOL finished) {
            
        }];
    }
}

@end
