//
//  NEOptimizeCollectionView.m
//  CHTCollectionViewWaterfallLayout
//
//  Created by Daniel on 2019/12/4.
//

#import "NEOptimizeCollectionView.h"

#import "NEOptimizeCollectionViewLayoutProtocol.h"

@implementation NEOptimizeCollectionView

- (id<NEOptimizeCollectionViewLayoutProtocol>)optimizeCollectionViewLayout {
    if ([self.collectionViewLayout conformsToProtocol:@protocol(NEOptimizeCollectionViewLayoutProtocol)]) {
        return (id<NEOptimizeCollectionViewLayoutProtocol>)self.collectionViewLayout;
    }
    else {
        return nil;
    }
}

- (void)insertSections:(NSIndexSet *)sections {
    [self.optimizeCollectionViewLayout insertSections:sections];
    [super insertSections:sections];
}

- (void)deleteSections:(NSIndexSet *)sections {
    [self.optimizeCollectionViewLayout deleteSections:sections];
    [super deleteSections:sections];
}

- (void)reloadSections:(NSIndexSet *)sections {
    [self.optimizeCollectionViewLayout reloadSections:sections];
    [super reloadSections:sections];
}

- (void)moveSection:(NSInteger)section toSection:(NSInteger)newSection {
    [self.optimizeCollectionViewLayout moveSection:section toSection:newSection];
    [super moveSection:section toSection:newSection];
}

- (void)insertItemsAtIndexPaths:(NSArray<NSIndexPath *> *)indexPaths {
    [self.optimizeCollectionViewLayout insertItemsAtIndexPaths:indexPaths];
    [super insertItemsAtIndexPaths:indexPaths];
}

- (void)deleteItemsAtIndexPaths:(NSArray<NSIndexPath *> *)indexPaths {
    [self.optimizeCollectionViewLayout deleteItemsAtIndexPaths:indexPaths];
    [super deleteItemsAtIndexPaths:indexPaths];
}

- (void)reloadItemsAtIndexPaths:(NSArray<NSIndexPath *> *)indexPaths {
    [self.optimizeCollectionViewLayout reloadItemsAtIndexPaths:indexPaths];
    [super reloadItemsAtIndexPaths:indexPaths];
}

- (void)moveItemAtIndexPath:(NSIndexPath *)indexPath toIndexPath:(NSIndexPath *)newIndexPath {
    [self.optimizeCollectionViewLayout moveItemAtIndexPath:indexPath toIndexPath:newIndexPath];
    [super moveItemAtIndexPath:indexPath toIndexPath:newIndexPath];
}

@end
