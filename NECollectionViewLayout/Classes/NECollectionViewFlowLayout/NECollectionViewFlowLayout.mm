//
//  NECollectionViewFlowLayout.m
//  Pods-NECollectionViewLayout_Example
//
//  Created by Daniel on 2019/11/28.
//

#include <vector>
#include <iostream>
#import "NECollectionViewFlowLayout.h"
#import "NECollectionViewFlowLayout+subhook.h"
#import "NECollectionViewDelegateResponds.h"
#import "NECollectionViewFlowLayoutCalculator.h"
#import "NECollectionViewFlowLayoutContext.h"
#import "NECollectionViewFlowLayoutAttributes.h"
#import "NECollectionViewFlowLayoutSectionContentScrollCollectionReusableView.h"

extern "C" NSString *const NECollectionElementKindSectionBackground = @"NECollectionElementKindSectionBackground";
extern "C" NSString *const NECollectionElementKindSectionScrollContent = @"NECollectionElementKindSectionScrollContent";

using namespace NE::CollectionViewFlowLayout;

@interface NECollectionViewFlowLayout () <NECollectionViewFlowLayoutAttributesDelegate>

@property (nonatomic, strong) NSMutableSet<NSIndexPath *> *insertedIndexPaths;
@property (nonatomic, strong) NSMutableSet<NSIndexPath *> *deletedIndexPaths;
@property (nonatomic, strong) NSArray<UICollectionViewUpdateItem *> *movedIndexPaths;
@property (nonatomic, strong) NSArray *sortedInsertedIndexPaths;

@end

@implementation NECollectionViewFlowLayout {
    ::Calculator _calculator;
}

+ (Class)invalidationContextClass {
    return [NECollectionViewFlowLayoutInvalidationContext class];
}

+ (Class)layoutAttributesClass {
    return [NECollectionViewFlowLayoutAttributes class];
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        [self registerClass:[NECollectionViewFlowLayoutSectionContentScrollCollectionReusableView class]
    forDecorationViewOfKind:NECollectionElementKindSectionScrollContent];
    }
    return self;
}

- (NE::CollectionViewFlowLayout::Calculator&)layout {
    return _calculator;
}

- (BOOL)invalidateFlowLayoutDelegateMetricsWhenUpdates {
    return _calculator.invalidateFlowLayoutDelegateMetricsWhenUpdates();
}

- (void)setInvalidateFlowLayoutDelegateMetricsWhenUpdates:(BOOL)invalidateFlowLayoutDelegateMetricsWhenUpdates {
    _calculator.setInvalidateFlowLayoutDelegateMetricsWhenUpdates(invalidateFlowLayoutDelegateMetricsWhenUpdates);
}

- (void)setPinToVisibleBoundsEnable:(BOOL)pinToVisibleBoundsEnable {
    if (_pinToVisibleBoundsEnable != pinToVisibleBoundsEnable) {
        _pinToVisibleBoundsEnable = pinToVisibleBoundsEnable;
        [self invalidateLayout];
    }
}

- (void)prepareLayout {
    [super prepareLayout];
    
    ::Context ctx(self);
    _calculator.setContext(std::move(ctx));
    _calculator.setBounds({ CGPointZero, self.collectionView.frame.size });
    _calculator.calculateLayoutIfNeeded();
}

- (NSArray<UICollectionViewLayoutAttributes *> *)layoutAttributesForElementsInRect:(CGRect)rect {
    ::LayoutCollection collection;
    _calculator.collectLayoutAttributesInRect(collection, rect, self.visibleRect);
    auto attributes = collection.attributes();
    
    return attributes;
}

- (UICollectionViewLayoutAttributes *)layoutAttributesForItemAtIndexPath:(NSIndexPath *)indexPath {
    return _calculator.itemAttributedAtIndexPath(indexPath);
}

- (nullable UICollectionViewLayoutAttributes *)layoutAttributesForSupplementaryViewOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)indexPath {
    return _calculator.supplementaryAtIndexPath(elementKind, indexPath, self.visibleRect);
}

- (UICollectionViewLayoutAttributes *)layoutAttributesForDecorationViewOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)indexPath {
    return _calculator.decorationAtIndexPath(elementKind, indexPath);
}

- (BOOL)shouldInvalidateLayoutForBoundsChange:(CGRect)newBounds {
    auto bounds = self.collectionView.bounds;
    return bounds.size.width != newBounds.size.width || self.pinToVisibleBoundsEnable;
}

- (UICollectionViewLayoutInvalidationContext *)invalidationContextForBoundsChange:(CGRect)newBounds {
    NECollectionViewFlowLayoutInvalidationContext *ctx = (NECollectionViewFlowLayoutInvalidationContext *)[super invalidationContextForBoundsChange:newBounds];
    auto bounds = self.collectionView.bounds;
    auto sizeChanged = bounds.size.width != newBounds.size.width;
    ctx.invalidateFlowLayoutAttributes = sizeChanged;
    ctx.invalidateFlowLayoutDelegateMetrics = sizeChanged;
    if (self.pinToVisibleBoundsEnable) {
        [ctx invalidateSupplementaryElementsOfKind:UICollectionElementKindSectionHeader
                                      atIndexPaths:_calculator.sectionHeaderPinToVisibleIndexes()];
        [ctx invalidateSupplementaryElementsOfKind:UICollectionElementKindSectionFooter
                                      atIndexPaths:_calculator.sectionFooterPinToVisibleIndexes()];
    }
    return ctx;
}

- (CGRect)visibleRect {
    using namespace NE;
    auto collectionView = self.collectionView;
    auto visibleRect = CGRect({collectionView.contentOffset, collectionView.frame.size});
    if (@available(iOS 11.0, *)) {
        return visibleRect + collectionView.adjustedContentInset;
    } else {
        return visibleRect + collectionView.contentInset;
    }
}

- (void)invalidateLayoutWithContext:(NECollectionViewFlowLayoutInvalidationContext *)context {
    NSParameterAssert([context isKindOfClass:NECollectionViewFlowLayoutInvalidationContext.class]);
    
//    NSLog(@"everthing: %@, datasource: %@, delegate: %@", @(context.invalidateEverything), @(context.invalidateDataSourceCounts), @(context.invalidateFlowLayoutDelegateMetrics));
    _calculator.invalidate((NECollectionViewFlowLayoutInvalidationContext *)context);
    [super invalidateLayoutWithContext:context];
}

- (CGSize)collectionViewContentSize {
    return _calculator.contentSize();
}

#pragma mark - animation
- (void)prepareForCollectionViewUpdates:(NSArray<UICollectionViewUpdateItem *> *)updateItems {
    _insertedIndexPaths = [NSMutableSet set];
    _deletedIndexPaths = [NSMutableSet set];
    NSMutableArray *movedIndexPaths = [NSMutableArray array];
    for (UICollectionViewUpdateItem *item in updateItems) {
        if (item.updateAction == UICollectionUpdateActionInsert) {
            [_insertedIndexPaths addObject:item.indexPathAfterUpdate];
        }
        else if (item.updateAction == UICollectionUpdateActionDelete) {
            [_deletedIndexPaths addObject:item.indexPathBeforeUpdate];
        }
        else if (item.updateAction == UICollectionUpdateActionMove) {
            [movedIndexPaths addObject:item];
        }
    }
    _movedIndexPaths = movedIndexPaths;
    _sortedInsertedIndexPaths = [_insertedIndexPaths sortedArrayUsingDescriptors:@[
        [NSSortDescriptor sortDescriptorWithKey:@"section" ascending:YES],
        [NSSortDescriptor sortDescriptorWithKey:@"item" ascending:YES]
    ]];
}

- (void)finalizeCollectionViewUpdates {
    self.insertedIndexPaths = nil;
    self.deletedIndexPaths = nil;
    self.sortedInsertedIndexPaths = nil;
    self.movedIndexPaths = nil;
}

- (NSIndexPath *)indexPathBeforeUpdatesForItemAtIndexPath:(NSIndexPath *)newIndexPath {
    if ([_insertedIndexPaths containsObject:newIndexPath]) {
        return nil;
    }
    for (UICollectionViewUpdateItem *item in _movedIndexPaths) {
        auto indexPathAfterUpdate = item.indexPathAfterUpdate;
        if ([indexPathAfterUpdate isEqual:newIndexPath]) {
            // Move item
            return item.indexPathBeforeUpdate;
        }
        else if (indexPathAfterUpdate.item == NSNotFound && indexPathAfterUpdate.section == newIndexPath.section) {
            // Move section
            return [NSIndexPath indexPathForItem:newIndexPath.item inSection:item.indexPathBeforeUpdate.section];
        }
    }
    
    auto section = newIndexPath.section;
    auto item = newIndexPath.item;
    NSInteger newSection = section;
    NSInteger newItem = item;
    
    for (NSIndexPath *insertedIndexPath in _sortedInsertedIndexPaths) {
        if (insertedIndexPath.item == NSNotFound && insertedIndexPath.section <= section) {
            // Insert section before
            newSection --;
        }
        else if (insertedIndexPath.section == section && insertedIndexPath.item <= item) {
            // Insert item before
            newItem --;
        }
    }
    
    for (NSIndexPath *deletedIndexPath in _deletedIndexPaths) {
        if (deletedIndexPath.item == NSNotFound && deletedIndexPath.section <= section) {
            // Delete section before
            newSection ++;
        }
        else if (deletedIndexPath.section == section && deletedIndexPath.item <= item) {
            // Delete item before
            newItem ++;
        }
    }
    
    if (newSection < 0 || newItem < 0) {
        return nil;
    }
    return [NSIndexPath indexPathForItem:newItem inSection:newSection];
}

- (BOOL)indexPathsBeforeUpdates:(NSMutableSet<NSIndexPath *> *)indexPathsBeforeUpdates containsItemIndexPath:(NSIndexPath *)itemIndexPath {
    for (NSIndexPath *indexPathBeforeUpdates in indexPathsBeforeUpdates) {
        if (indexPathBeforeUpdates.item == NSNotFound && indexPathBeforeUpdates.section == itemIndexPath.section) {
            return YES;
        }
        else if (indexPathBeforeUpdates.section == itemIndexPath.section && indexPathBeforeUpdates.item == itemIndexPath.item) {
            return YES;
        }
    }
    return NO;
}

- (nullable UICollectionViewLayoutAttributes *)initialLayoutAttributesForAppearingItemAtIndexPath:(NSIndexPath *)itemIndexPath {
    if ([self indexPathsBeforeUpdates:self.insertedIndexPaths containsItemIndexPath:itemIndexPath] && self.appearenceAnimator) {
        auto prevIndexPath = [self indexPathBeforeUpdatesForItemAtIndexPath:itemIndexPath];
        UICollectionViewLayoutAttributes *prevAttr = nil;
        if (prevIndexPath) {
            prevAttr = _calculator.prevItemAttributedAtIndexPath(prevIndexPath);
        }
        if ([self.appearenceAnimator respondsToSelector:@selector(layout:initialLayoutAttributesForAppearingItemAtIndexPath:previousAttributes:)]) {
            return [self.appearenceAnimator layout:self initialLayoutAttributesForAppearingItemAtIndexPath:itemIndexPath previousAttributes:prevAttr];
        }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        else if ([self.appearenceAnimator respondsToSelector:@selector(layout:initialLayoutAttributesForAppearingItemAtIndexPath:)]) {
            return [self.appearenceAnimator layout:self initialLayoutAttributesForAppearingItemAtIndexPath:itemIndexPath];
        }
#pragma GCC diagnostic pop
        return prevAttr;
    }
    
    return [super initialLayoutAttributesForAppearingItemAtIndexPath:itemIndexPath];
}

- (nullable UICollectionViewLayoutAttributes *)finalLayoutAttributesForDisappearingItemAtIndexPath:(NSIndexPath *)itemIndexPath {
    if ([self indexPathsBeforeUpdates:self.deletedIndexPaths containsItemIndexPath:itemIndexPath] && self.appearenceAnimator) {
        UICollectionViewLayoutAttributes *animAttr = nil;
        if ([self.appearenceAnimator respondsToSelector:@selector(layout:finalLayoutAttributesForDisappearingItemAtIndexPath:previousAttributes:)]) {
            auto prevAttr = _calculator.prevItemAttributedAtIndexPath(itemIndexPath);
            animAttr = [self.appearenceAnimator layout:self finalLayoutAttributesForDisappearingItemAtIndexPath:itemIndexPath previousAttributes:prevAttr];
        }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        else if ([self.appearenceAnimator respondsToSelector:@selector(layout:finalLayoutAttributesForDisappearingItemAtIndexPath:)]) {
            animAttr = [self.appearenceAnimator layout:self finalLayoutAttributesForDisappearingItemAtIndexPath:itemIndexPath];
        }
#pragma GCC diagnostic pop
        return animAttr ?: [super finalLayoutAttributesForDisappearingItemAtIndexPath:itemIndexPath];
    }
    
    return [super finalLayoutAttributesForDisappearingItemAtIndexPath:itemIndexPath];
}

- (UICollectionViewLayoutAttributes *)initialLayoutAttributesForAppearingSupplementaryElementOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)elementIndexPath {
    auto prevIndexPath = [self indexPathBeforeUpdatesForItemAtIndexPath:elementIndexPath];
    auto prevAttr = _calculator.prevSupplementaryAtIndexPath(elementKind, prevIndexPath, self.visibleRect);
    if ([self.supplementaryAppearenceAnimator respondsToSelector:@selector(layout:initialLayoutAttributesForAppearingItemAtIndexPath:elementKind:previousAttributes:)]) {
        return [self.supplementaryAppearenceAnimator layout:self
         initialLayoutAttributesForAppearingItemAtIndexPath:elementIndexPath
                                                elementKind:elementKind
                                         previousAttributes:prevAttr];
    }
    return prevAttr;
}

- (UICollectionViewLayoutAttributes *)finalLayoutAttributesForDisappearingSupplementaryElementOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)elementIndexPath {
    if ([self.supplementaryAppearenceAnimator respondsToSelector:@selector(layout:finalLayoutAttributesForDisappearingItemAtIndexPath:elementKind:previousAttributes:)]) {
        auto prevAttr = _calculator.prevSupplementaryAtIndexPath(elementKind, elementIndexPath, self.visibleRect);
        return [self.supplementaryAppearenceAnimator layout:self
        finalLayoutAttributesForDisappearingItemAtIndexPath:elementIndexPath
                                                elementKind:elementKind
                                         previousAttributes:prevAttr];
    }
    
    return _calculator.supplementaryAtIndexPath(elementKind, elementIndexPath, self.visibleRect);
}

- (UICollectionViewLayoutAttributes *)initialLayoutAttributesForAppearingDecorationElementOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)decorationIndexPath {
    auto prevIndexPath = [self indexPathBeforeUpdatesForItemAtIndexPath:decorationIndexPath];
    auto prevAttr = _calculator.prevDecorationAtIndexPath(elementKind, prevIndexPath);
    if ([self.decorationAppearenceAnimator respondsToSelector:@selector(layout:initialLayoutAttributesForAppearingItemAtIndexPath:elementKind:previousAttributes:)]) {
        return [self.decorationAppearenceAnimator layout:self
      initialLayoutAttributesForAppearingItemAtIndexPath:decorationIndexPath
                                             elementKind:elementKind
                                      previousAttributes:prevAttr];
    }
    return prevAttr;
}

- (UICollectionViewLayoutAttributes *)finalLayoutAttributesForDisappearingDecorationElementOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)decorationIndexPath {
    if ([self.decorationAppearenceAnimator respondsToSelector:@selector(layout:finalLayoutAttributesForDisappearingItemAtIndexPath:elementKind:previousAttributes:)]) {
        auto prevAttr = _calculator.prevDecorationAtIndexPath(elementKind, decorationIndexPath);
        return [self.decorationAppearenceAnimator layout:self
     finalLayoutAttributesForDisappearingItemAtIndexPath:decorationIndexPath
                                             elementKind:elementKind
                                      previousAttributes:prevAttr];
    }
    
    return _calculator.decorationAtIndexPath(elementKind, decorationIndexPath);
}

#pragma mark - reorder
//- (NSIndexPath *)targetIndexPathForInteractivelyMovingItem:(NSIndexPath *)previousIndexPath withPosition:(CGPoint)position API_AVAILABLE(ios(9.0)) {
//    NSLog(@"index");
//    return _calculator.targetIndexPathAtPosition(position) ?: previousIndexPath;
//}
//
//- (UICollectionViewLayoutAttributes *)layoutAttributesForInteractivelyMovingItemAtIndexPath:(NSIndexPath *)indexPath withTargetPosition:(CGPoint)position API_AVAILABLE(ios(9.0)) {
//    NSLog(@"moving");
//    UICollectionViewLayoutAttributes *att = [super layoutAttributesForInteractivelyMovingItemAtIndexPath:indexPath withTargetPosition:position];
//    att.transform = CGAffineTransformScale(CGAffineTransformIdentity, 1.2, 1.2);
//    return att;
//}
//
//- (UICollectionViewLayoutInvalidationContext *)invalidationContextForInteractivelyMovingItems:(NSArray<NSIndexPath *> *)targetIndexPaths withTargetPosition:(CGPoint)targetPosition previousIndexPaths:(NSArray<NSIndexPath *> *)previousIndexPaths previousPosition:(CGPoint)previousPosition API_AVAILABLE(ios(9.0)) {
//    NSLog(@"invidate");
//    NECollectionViewFlowLayoutInvalidationContext *ctx = [[[[self class] invalidationContextClass] alloc] init];
//    ctx.invalidateFlowLayoutDelegateMetrics = NO;
//    return ctx;
//}
//
//- (UICollectionViewLayoutInvalidationContext *)invalidationContextForEndingInteractiveMovementOfItemsToFinalIndexPaths:(NSArray<NSIndexPath *> *)indexPaths previousIndexPaths:(NSArray<NSIndexPath *> *)previousIndexPaths movementCancelled:(BOOL)movementCancelled API_AVAILABLE(ios(9.0)) {
//    NECollectionViewFlowLayoutInvalidationContext *ctx = [[[[self class] invalidationContextClass] alloc] init];
//    ctx.invalidateFlowLayoutDelegateMetrics = NO;
//    return ctx;
//}

#pragma mark - section api
- (CGPoint)contentOffsetForSectionAtIndex:(NSInteger)section {
    if (section < _calculator.sections().size()) {
        auto& sec = _calculator.sections().at(section);
        return sec.contentOffset();
    }
    else {
        @try {
            [[NSException exceptionWithName:@"Invalidate section" reason:@"Invalidate section" userInfo:nil] raise];
        }
        @catch (NSException *exception) { }
        return CGPointZero;
    }
}

- (void)setContentOffset:(CGPoint)contentOffset forSectionAtIndex:(NSInteger)section {
    if (section < _calculator.sections().size()) {
        auto& sec = const_cast<Section&>(_calculator.sections().at(section));
        sec.setContentOffset(contentOffset);
    }
    else {
        @try {
            [[NSException exceptionWithName:@"Invalidate section" reason:@"Invalidate section" userInfo:nil] raise];
        }
        @catch (NSException *exception) { }
    }
}

- (CGRect)frameForSectionAtIndex:(NSInteger)section {
    if (section < _calculator.sections().size()) {
        auto& sec = _calculator.sections().at(section);
        return sec.frame();
    }
    else {
        @try {
            [[NSException exceptionWithName:@"Invalidate section" reason:@"Invalidate section" userInfo:nil] raise];
        }
        @catch (NSException *exception) { }
        return CGRectZero;
    }
}

#pragma mark - Edit

static inline
std::set<NSUInteger> CSectionFromIndexSet(NSIndexSet *indexSet) {
    std::vector<NSUInteger> cSections;
    cSections.resize(indexSet.count);
    [indexSet getIndexes:cSections.data() maxCount:cSections.size() inIndexRange:nullptr];
    std::set<NSUInteger> set(cSections.begin(), cSections.end());
    return set;
}

static inline
std::set<NE::IndexPath> CIndexPathsFromIndexPaths(NSArray<NSIndexPath *> *indexPaths) {
    std::set<NE::IndexPath> cIndexPaths;
    for (NSIndexPath *idx in indexPaths) {
        cIndexPaths.emplace(idx);
    }
    return cIndexPaths;
}

- (void)insertSections:(NSIndexSet *)sections {
    if (!self.invalidateFlowLayoutDelegateMetricsWhenUpdates) {
        auto cSections = CSectionFromIndexSet(sections);
        _calculator.updates().insertSections(std::move(cSections));
    }
}

- (void)deleteSections:(NSIndexSet *)sections {
    if (!self.invalidateFlowLayoutDelegateMetricsWhenUpdates) {
        auto cSections = CSectionFromIndexSet(sections);
        _calculator.updates().deleteSections(std::move(cSections));
    }
}

- (void)reloadSections:(NSIndexSet *)sections {
    if (!self.invalidateFlowLayoutDelegateMetricsWhenUpdates) {
        auto cSections = CSectionFromIndexSet(sections);
        _calculator.updates().reloadSections(std::move(cSections));
    }
}

- (void)moveSection:(NSInteger)section toSection:(NSInteger)newSection {
    if (!self.invalidateFlowLayoutDelegateMetricsWhenUpdates) {
        _calculator.updates().moveSection(section, newSection);
    }
}

- (void)insertItemsAtIndexPaths:(NSArray<NSIndexPath *> *)indexPaths {
    if (!self.invalidateFlowLayoutDelegateMetricsWhenUpdates) {
        auto cIndexPaths = CIndexPathsFromIndexPaths(indexPaths);
        _calculator.updates().insertItems(std::move(cIndexPaths));
    }
}

- (void)deleteItemsAtIndexPaths:(NSArray<NSIndexPath *> *)indexPaths {
    if (!self.invalidateFlowLayoutDelegateMetricsWhenUpdates) {
        auto cIndexPaths = CIndexPathsFromIndexPaths(indexPaths);
        _calculator.updates().deleteItems(std::move(cIndexPaths));
    }
}

- (void)reloadItemsAtIndexPaths:(NSArray<NSIndexPath *> *)indexPaths {
    if (!self.invalidateFlowLayoutDelegateMetricsWhenUpdates) {
        auto cIndexPaths = CIndexPathsFromIndexPaths(indexPaths);
        _calculator.updates().reloadItems(std::move(cIndexPaths));
    }
}

- (void)moveItemAtIndexPath:(NSIndexPath *)indexPath toIndexPath:(NSIndexPath *)newIndexPath {
    if (!self.invalidateFlowLayoutDelegateMetricsWhenUpdates) {
        NE::IndexPath from(indexPath);
        NE::IndexPath to(newIndexPath);
        _calculator.updates().moveItem(from, to);
    }
}

#pragma mark - NECollectionViewFlowLayoutAttributesDelegate
- (void)collectionViewFlowLayoutAttributesSectionDidScrollWithContentOffset:(CGPoint)offset atIndexPath:(nonnull NSIndexPath *)indexPath {
    NECollectionViewFlowLayoutInvalidationContext *ctx = [[[[self class] invalidationContextClass] alloc] init];
    ctx.invalidateFlowLayoutAttributes = NO;
    ctx.invalidateFlowLayoutDelegateMetrics = NO;
    [ctx invalidateScrollOffset:offset forSectionAtIndex:indexPath.section];
    [self invalidateLayoutWithContext:ctx];
    
    auto delegate = self.collectionView.delegate;
    if ([delegate respondsToSelector:@selector(collectionView:layout:scrollViewDidScrollToContentOffset:forSectionAtIndex:)]) {
        [(id<NECollectionViewDelegateFlowLayout>)delegate collectionView:self.collectionView
                                                                  layout:self
                                      scrollViewDidScrollToContentOffset:offset
                                                       forSectionAtIndex:indexPath.section];
    }
}

@end
