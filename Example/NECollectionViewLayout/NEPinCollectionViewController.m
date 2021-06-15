//
//  NEPinCollectionViewController.m
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
#import "NEPinCollectionViewController.h"

@interface NEPinCollectionViewController () <NECollectionViewDelegateFlowLayout>
@property (nonatomic, assign) NECollectionViewFlowLayoutPinToVisibleBounds style;
@end

@implementation NEPinCollectionViewController

- (instancetype)init
{
    NECollectionViewFlowLayout *layout = [NECollectionViewFlowLayout new];
    layout.pinToVisibleBoundsEnable = YES;
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
    
    __auto_type pin = [[UIBarButtonItem alloc] initWithTitle:@"pin" style:UIBarButtonItemStylePlain target:self action:@selector(pin)];
    __auto_type style = [[UIBarButtonItem alloc] initWithTitle:@"style" style:UIBarButtonItemStylePlain target:self action:@selector(pinStyle)];
    self.navigationItem.rightBarButtonItems = @[pin, style];
    
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
    
}

#pragma mark <UICollectionViewDataSource>

- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView {
    return 4;
}


- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return 40;
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    NETextCollectionViewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:NSStringFromClass(NETextCollectionViewCell.class) forIndexPath:indexPath];
    cell.backgroundColor = [UIColor grayColor];
    cell.textLabel.text = [NSString stringWithFormat:@"%@-%@", @(indexPath.section).stringValue, @(indexPath.item).stringValue];
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
    return  CGSizeMake(collectionView.frame.size.width / 6, indexPath.item % 3 ? 44 : 88);
}

- (NECollectionViewFlowLayoutAlignment)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout alignVerticalForSectionAtIndex:(NSInteger)section {
    return section % 3;
}

- (NECollectionViewFlowLayoutAlignment)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout alignHorizontalForSectionAtIndex:(NSInteger)section {
    return section % 4;
}

- (NECollectionViewFlowLayoutPinToVisibleBounds)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout headerPinToVisibleBoundsForSectionAtIndex:(NSInteger)section {
    if (section == 1) {
        return self.style;
    }
    return NECollectionViewFlowLayoutPinToVisibleBoundsNone;
}

- (NECollectionViewFlowLayoutPinToVisibleBounds)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout footerPinToVisibleBoundsForSectionAtIndex:(NSInteger)section {
    if (section == 1) {
        return self.style;
    }
    return NECollectionViewFlowLayoutPinToVisibleBoundsNone;
}


- (BOOL)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout backgroundVisibleForSectionAtIndex:(NSInteger)section {
    return YES;
}

- (BOOL)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout backgroundIncludeSupplementarysForSectionAtIndex:(NSInteger)section {
    return NO;
}

- (UIEdgeInsets)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)layout backgroundInsetsForSectionAtIndex:(NSInteger)section {
    return UIEdgeInsetsMake(-10, -10, -10, -10);
}

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath {

}

- (void)pin {
    NECollectionViewFlowLayout *layout = (NECollectionViewFlowLayout *)self.collectionViewLayout;
    layout.pinToVisibleBoundsEnable = !layout.pinToVisibleBoundsEnable;
    [self updateTitle];
}

- (void)pinStyle {
    self.style ++;
    if (self.style > NECollectionViewFlowLayoutPinToVisibleBoundsAlways) {
        self.style = NECollectionViewFlowLayoutPinToVisibleBoundsNone;
    }
    [self.collectionViewLayout invalidateLayout];
    [self updateTitle];
}

- (void)updateTitle {
    self.title = [NSString stringWithFormat:@"%@-%@",
                  @([(NECollectionViewFlowLayout *)self.collectionViewLayout pinToVisibleBoundsEnable]),
                  NECollectionViewFlowLayoutPinToVisibleBoundsToReadable(self.style)];
}

@end
