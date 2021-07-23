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
/*
namespace {
struct CollectionViewUpdater {
    struct Item {
        Item() : inserted_{true} { }
        Item(NSInteger index) : indexBefore_{index} {}
        bool inserted() { return inserted_; }
        NSInteger indexBefore() { return indexBefore_; }
    private:
        bool inserted_ = false;
        NSInteger indexBefore_;
    };
    struct Section {
        Section() : inserted_{true} { }
        Section(NSInteger index, NSInteger count) : indexBefore_{index}, count_{count} {
            items_.reserve(count);
            for (NSInteger i = 0; i < count; i++) {
                items_.emplace_back(i);
            }
        }
        bool inserted() { return inserted_; }
        NSInteger indexBefore() { return indexBefore_; }
        Item& item(NSInteger index) { return items_.at(index); }
        void insert(NSInteger item) {
            if (item <= items_.size()) {
                items_.emplace(items_.begin() + item);
            }
        }
        void remove(NSInteger item) {
            if (item < items_.size()) {
                items_.erase(items_.begin() + item);
            }
        }
        void move(NSInteger fromItem, NSInteger toItem) {
            if (fromItem < items_.size() && toItem < items_.size()) {
                std::iter_swap(items_.begin() + fromItem, items_.begin() + toItem);
            }
        }
    private:
        friend class CollectionViewUpdater;
        bool inserted_ = false;
        NSInteger indexBefore_;
        NSInteger count_;
        std::vector<Item> items_;
    };
    
    using SectionsType = std::vector<NE::CollectionViewFlowLayout::Section>;

    void reset(const SectionsType& sections) {
        count_ = static_cast<NSInteger>(sections.size());
        sections_.clear();
        sections_.reserve(static_cast<size_t>(count_));
        for (NSInteger i = 0; i < count_; i++) {
            sections_.emplace_back(i, sections.at(i).itemCount());
        }
    }
    
    void insert(NSInteger section) {
        if (section <= sections_.size()) {
            sections_.emplace(sections_.begin() + section);
        }
    }
    
    void insert(NSInteger section, NSInteger item) {
        if (section < sections_.size()) {
            this->section(section).insert(item);
        }
    }
    
    void remove(NSInteger section) {
        if (section < sections_.size()) {
            sections_.erase(sections_.begin() + section);
        }
    }
    
    void remove(NSInteger section, NSInteger item) {
        if (section < sections_.size()) {
            this->section(section).remove(item);
        }
    }
    
    void move(NSInteger fromSection, NSInteger toSection) {
        if (fromSection < sections_.size() && toSection < sections_.size()) {
            std::iter_swap(sections_.begin() + fromSection, sections_.begin() + toSection);
        }
    }
    
    void move(NSInteger fromSection, NSInteger fromItem, NSInteger toSection, NSInteger toItem) {
        if (fromSection == toSection && fromSection < sections_.size()) {
            sections_.at(fromSection).move(fromItem, toItem);
        }
    }
    
    using IndexPath = std::pair<NSInteger, NSInteger>;
    std::optional<IndexPath> findIndexPathAfterUpdate(IndexPath oldIndexPath) {
        for (NSInteger i = 0; i < sections_.size(); i++) {
            auto& section = this->section(i);
            if (!section.inserted() && section.indexBefore() == oldIndexPath.first) {
                for (NSInteger j = 0; j < sections_.size(); j++) {
                    auto& item = section.item(j);
                    if (!item.inserted() && item.indexBefore() == oldIndexPath.second) {
                        return IndexPath{ i, j };
                    }
                }
            }
        }
        return std::nullopt;
    }
    
    std::optional<IndexPath> findIndexPathBeforeUpdate(IndexPath newIndexPath) {
        if (newIndexPath.first < sections_.size()) {
            auto& section = this->section(newIndexPath.first);
            if (!section.inserted() && newIndexPath.second < section.items_.size()) {
                auto& item = section.item(newIndexPath.second);
                if (!item.inserted()) {
                    return IndexPath{ section.indexBefore(), item.indexBefore() };
                }
            }
        }
        
        return std::nullopt;
    }
    
    void update(NSArray<UICollectionViewUpdateItem *> *updateItems) {
        std::vector<NSInteger> insertedSections;
        std::vector<NSInteger> deletedSections;
        std::vector<std::pair<NSInteger, NSInteger>> movedSections;
        std::vector<CollectionViewUpdater::IndexPath> insertedIndexPaths;
        std::vector<CollectionViewUpdater::IndexPath> deletedIndexPaths;
        std::vector<std::pair<CollectionViewUpdater::IndexPath, CollectionViewUpdater::IndexPath>> movedIndexPaths;
        
        for (UICollectionViewUpdateItem *item in updateItems) {
            switch (item.updateAction) {
                case UICollectionUpdateActionInsert: {
                    auto section = item.indexPathAfterUpdate.section;
                    auto it = item.indexPathAfterUpdate.item;
                    if (it == NSNotFound) {
                        insertedSections.emplace_back(section);
                    }
                    else {
                        insertedIndexPaths.emplace_back(section, it);
                    }
                }
                    break;
                case UICollectionUpdateActionDelete: {
                    auto section = item.indexPathBeforeUpdate.section;
                    auto it = item.indexPathBeforeUpdate.item;
                    if (it == NSNotFound) {
                        deletedSections.emplace_back(section);
                    }
                    else {
                        deletedIndexPaths.emplace_back(section, it);
                    }
                }
                    break;
                case UICollectionUpdateActionMove: {
                    auto fs = item.indexPathBeforeUpdate.section;
                    auto fi = item.indexPathBeforeUpdate.item;
                    auto ts = item.indexPathAfterUpdate.section;
                    auto ti = item.indexPathAfterUpdate.item;
                    if (fi == NSNotFound || ti == NSNotFound) {
                        movedSections.emplace_back(fs, ts);
                    }
                    else {
                        movedIndexPaths.emplace_back(CollectionViewUpdater::IndexPath{fs, fi},
                                                     CollectionViewUpdater::IndexPath{ts, ti});
                    }
                }
                    break;
                    
                default:
                    break;
            }
        }
        
        if (insertedSections.size()) {
            std::sort(insertedSections.begin(), insertedSections.end());
            for (auto section : insertedSections) {
                insert(section);
            }
        }
        if (deletedSections.size()) {
            std::sort(deletedSections.begin(), deletedSections.end(), std::greater<>());
            for (auto section : deletedSections) {
                remove(section);
            }
        }
        if (movedSections.size()) {
            for (auto [from, to] : movedSections) {
                move(from, to);
            }
        }
        struct IndexPathLesser {
            bool operator()(CollectionViewUpdater::IndexPath& l, CollectionViewUpdater::IndexPath &r) {
                return l.first < r.first || (l.first == r.first && l.second < r.second);
            }
        };
        if (insertedIndexPaths.size()) {
            std::sort(insertedIndexPaths.begin(), insertedIndexPaths.end(), IndexPathLesser());
            for (auto& indexPath : insertedIndexPaths) {
                insert(indexPath.first, indexPath.second);
            }
        }
        struct IndexPathGreater {
            bool operator()(CollectionViewUpdater::IndexPath& l, CollectionViewUpdater::IndexPath &r) {
                return l.first > r.first || (l.first == r.first && l.second > r.second);
            }
        };
        if (deletedIndexPaths.size()) {
            std::sort(deletedIndexPaths.begin(), deletedIndexPaths.end(), IndexPathLesser());
            for (auto& indexPath : deletedIndexPaths) {
                remove(indexPath.first, indexPath.second);
            }
        }
        if (movedIndexPaths.size()) {
            for (auto& [from, to] : movedIndexPaths) {
                move(from.first, from.second, to.first, to.second);
            }
        }
    }
    
    Section& section(NSInteger index) {
        return sections_.at(index);
    }
private:
    std::vector<Section> sections_;
    NSInteger count_;
};
}
*/
@interface NECollectionViewFlowLayout () <NECollectionViewFlowLayoutAttributesDelegate>

@property (nonatomic, strong) NSMutableSet<NSIndexPath *> *insertedIndexPaths;
@property (nonatomic, strong) NSMutableSet<NSIndexPath *> *deletedIndexPaths;

@property (nonatomic, strong) NSMutableSet<NSIndexPath *> *insertedSectionIndexPaths;
@property (nonatomic, strong) NSMutableSet<NSIndexPath *> *deletedSectionIndexPaths;

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
    _insertedSectionIndexPaths = [NSMutableSet set];
    _deletedSectionIndexPaths = [NSMutableSet set];
    for (UICollectionViewUpdateItem *item in updateItems) {
        if (item.updateAction == UICollectionUpdateActionInsert) {
            auto idx = item.indexPathAfterUpdate;
            [_insertedIndexPaths addObject:idx];
            if (idx.item == NSNotFound) {
                [_insertedSectionIndexPaths addObject:[NSIndexPath indexPathForItem:0 inSection:idx.section]];
            }
        }
        else if (item.updateAction == UICollectionUpdateActionDelete) {
            auto idx = item.indexPathBeforeUpdate;
            [_deletedIndexPaths addObject:idx];
            if (idx.item == NSNotFound) {
                [_deletedSectionIndexPaths addObject:[NSIndexPath indexPathForItem:0 inSection:idx.section]];
            }
        }
        else if (item.updateAction == UICollectionUpdateActionReload) {
            auto idx = item.indexPathBeforeUpdate;
            if (idx.item == NSNotFound) {
                auto sectionIdx = [NSIndexPath indexPathForItem:0 inSection:idx.section];
                [_insertedSectionIndexPaths addObject:sectionIdx];
                [_deletedSectionIndexPaths addObject:sectionIdx];
            }
        }
        else if (item.updateAction == UICollectionUpdateActionMove) {
            // Move equals delete then insert
            {
                auto idx = item.indexPathAfterUpdate;
                if (idx.item == NSNotFound) {
                    [_insertedSectionIndexPaths addObject:[NSIndexPath indexPathForItem:0 inSection:idx.section]];
                }
            }
            {
                auto idx = item.indexPathBeforeUpdate;
                if (idx.item == NSNotFound) {
                    [_deletedSectionIndexPaths addObject:[NSIndexPath indexPathForItem:0 inSection:idx.section]];
                }
            }
        }
    }
}

- (void)finalizeCollectionViewUpdates {
    self.insertedIndexPaths = nil;
    self.deletedIndexPaths = nil;
    self.insertedSectionIndexPaths = nil;
    self.deletedSectionIndexPaths = nil;
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
        UICollectionViewLayoutAttributes *prevAttr = _calculator.itemAttributedAtIndexPath(itemIndexPath);
        if ([self.appearenceAnimator respondsToSelector:@selector(layout:initialLayoutAttributesForAppearingItemAtIndexPath:previousAttributes:)]) {
            return [self.appearenceAnimator layout:self initialLayoutAttributesForAppearingItemAtIndexPath:itemIndexPath previousAttributes:prevAttr];
        }
        return prevAttr;
    }
    
    return _calculator.itemAttributedAtIndexPath(itemIndexPath) ?: [super initialLayoutAttributesForAppearingItemAtIndexPath:itemIndexPath];
}

- (nullable UICollectionViewLayoutAttributes *)finalLayoutAttributesForDisappearingItemAtIndexPath:(NSIndexPath *)itemIndexPath {
    if ([self indexPathsBeforeUpdates:self.deletedIndexPaths containsItemIndexPath:itemIndexPath] && self.appearenceAnimator) {
        UICollectionViewLayoutAttributes *animAttr = nil;
        if ([self.appearenceAnimator respondsToSelector:@selector(layout:finalLayoutAttributesForDisappearingItemAtIndexPath:previousAttributes:)]) {
            auto prevAttr = _calculator.prevItemAttributedAtIndexPath(itemIndexPath);
            animAttr = [self.appearenceAnimator layout:self finalLayoutAttributesForDisappearingItemAtIndexPath:itemIndexPath previousAttributes:prevAttr];
        }
        return animAttr ?: [super finalLayoutAttributesForDisappearingItemAtIndexPath:itemIndexPath];
    }
    
    return _calculator.prevItemAttributedAtIndexPath(itemIndexPath) ?: [super finalLayoutAttributesForDisappearingItemAtIndexPath:itemIndexPath];
}

- (UICollectionViewLayoutAttributes *)initialLayoutAttributesForAppearingSupplementaryElementOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)elementIndexPath {
    UICollectionViewLayoutAttributes *prevAttr = _calculator.supplementaryAtIndexPath(elementKind, elementIndexPath, self.visibleRect);
    if ([self.supplementaryAppearenceAnimator respondsToSelector:@selector(layout:initialLayoutAttributesForAppearingItemAtIndexPath:elementKind:previousAttributes:)]) {
        return [self.supplementaryAppearenceAnimator layout:self
         initialLayoutAttributesForAppearingItemAtIndexPath:elementIndexPath
                                                elementKind:elementKind
                                         previousAttributes:prevAttr];
    }
    return prevAttr ?: [super initialLayoutAttributesForAppearingSupplementaryElementOfKind:elementKind atIndexPath:elementIndexPath];
}

- (UICollectionViewLayoutAttributes *)finalLayoutAttributesForDisappearingSupplementaryElementOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)elementIndexPath {
    UICollectionViewLayoutAttributes *afterAttr = _calculator.prevSupplementaryAtIndexPath(elementKind, elementIndexPath, self.visibleRect);
    if ([self.supplementaryAppearenceAnimator respondsToSelector:@selector(layout:initialLayoutAttributesForAppearingItemAtIndexPath:elementKind:previousAttributes:)]) {
        return [self.supplementaryAppearenceAnimator layout:self
         initialLayoutAttributesForAppearingItemAtIndexPath:elementIndexPath
                                                elementKind:elementKind
                                         previousAttributes:afterAttr];
    }
    return afterAttr ?: [super initialLayoutAttributesForAppearingSupplementaryElementOfKind:elementKind atIndexPath:elementIndexPath];
}

- (UICollectionViewLayoutAttributes *)initialLayoutAttributesForAppearingDecorationElementOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)decorationIndexPath {
    auto prevAttr = _calculator.decorationAtIndexPath(elementKind, decorationIndexPath);
    if ([self.decorationAppearenceAnimator respondsToSelector:@selector(layout:initialLayoutAttributesForAppearingItemAtIndexPath:elementKind:previousAttributes:)]) {
        return [self.decorationAppearenceAnimator layout:self
      initialLayoutAttributesForAppearingItemAtIndexPath:decorationIndexPath
                                             elementKind:elementKind
                                      previousAttributes:prevAttr];
    }
    return prevAttr;
}

- (UICollectionViewLayoutAttributes *)finalLayoutAttributesForDisappearingDecorationElementOfKind:(NSString *)elementKind atIndexPath:(NSIndexPath *)decorationIndexPath {
    auto prevAttr = _calculator.prevDecorationAtIndexPath(elementKind, decorationIndexPath);
    if ([self.decorationAppearenceAnimator respondsToSelector:@selector(layout:finalLayoutAttributesForDisappearingItemAtIndexPath:elementKind:previousAttributes:)]) {
        return [self.decorationAppearenceAnimator layout:self
     finalLayoutAttributesForDisappearingItemAtIndexPath:decorationIndexPath
                                             elementKind:elementKind
                                      previousAttributes:prevAttr];
    }
    
    return prevAttr;
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


- (NSArray<NSIndexPath *> *)indexPathsToDeleteForSupplementaryViewOfKind:(NSString *)elementKind API_AVAILABLE(ios(7.0)) {
    return _deletedSectionIndexPaths.allObjects;
}

- (NSArray<NSIndexPath *> *)indexPathsToDeleteForDecorationViewOfKind:(NSString *)elementKind API_AVAILABLE(ios(7.0)) {
    return _deletedSectionIndexPaths.allObjects;
}

- (NSArray<NSIndexPath *> *)indexPathsToInsertForSupplementaryViewOfKind:(NSString *)elementKind API_AVAILABLE(ios(7.0)) {
    return _insertedSectionIndexPaths.allObjects;
}

- (NSArray<NSIndexPath *> *)indexPathsToInsertForDecorationViewOfKind:(NSString *)elementKind API_AVAILABLE(ios(7.0)) {
    return _insertedSectionIndexPaths.allObjects;
}

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
