//
//  NECollectionViewFlowLayoutCalculator.h
//  Pods
//
//  Created by Daniel on 2019/11/28.
//

#ifndef NECollectionViewFlowLayoutCalculator_h
#define NECollectionViewFlowLayoutCalculator_h

#include <UIKit/UIKit.h>
#include <vector>
#include <set>
#include "NECollectionViewLayoutHelpers.h"
#include "NECollectionViewFlowLayoutContext.h"
#include "NECollectionViewFlowLayoutSection.h"
#include "NECollectionViewFlowLayoutInvalidation.h"
#import "NECollectionViewFlowLayoutInvalidationContext.h"
#include "NECollectionViewFlowLayoutCollection.h"
#include "NECollectionViewUpdates.h"

namespace NE::CollectionViewFlowLayout {

class Calculator : public Container {
public:
    using Updates = CollectionView::Updates;
    
    Calculator() {}
    ~Calculator() override {}
    
    void setBounds(const CGRect& bounds) {
        if (bounds_ != bounds) {
            bounds_ = bounds;
            markLayoutDirty();
        }
    }
    
    void setContext(Context&& ctx) { context_ = ctx; }
    Context& context() { return context_; }
    
    Updates& updates() { return updates_; }
    
    const auto& sections() const { return sections_; }
    
    bool invalidateFlowLayoutDelegateMetricsWhenUpdates() {
        return invalidateFlowLayoutDelegateMetricsWhenUpdates_;
    }
    void setInvalidateFlowLayoutDelegateMetricsWhenUpdates(bool b) {
        invalidateFlowLayoutDelegateMetricsWhenUpdates_ = b;
    }
    
    void invalidate(NECollectionViewFlowLayoutInvalidationContext *ctx) {
        if (ctx.invalidateEverything) invalidation_.invalidateEverything();
        if (ctx.invalidateDataSourceCounts) invalidation_.invalidateDataSourceCounts();
        if (ctx.invalidateFlowLayoutDelegateMetrics) invalidation_.invalidateFlowLayoutDelegateMetrics();
        if (ctx.invalidateFlowLayoutAttributes) invalidation_.invalidateFlowLayoutAttributes();
        if (ctx.invalidatedItemIndexPaths.count) {
            for (NSIndexPath *idx in ctx.invalidatedItemIndexPaths) {
                invalidation_.invalidateItem(idx);
            }
        }
        if (ctx.invalidatedSupplementaryIndexPaths.count) {
            [ctx.invalidatedSupplementaryIndexPaths enumerateKeysAndObjectsUsingBlock:^(NSString * _Nonnull key, NSArray<NSIndexPath *> * _Nonnull obj, BOOL * _Nonnull stop) {
                for (NSIndexPath *idx in obj) {
                    invalidation_.invalidateSumplementary(key, idx);
                }
            }];
        }
        if (ctx.invalidatedDecorationIndexPaths.count) {
            [ctx.invalidatedDecorationIndexPaths enumerateKeysAndObjectsUsingBlock:^(NSString * _Nonnull key, NSArray<NSIndexPath *> * _Nonnull obj, BOOL * _Nonnull stop) {
                for (NSIndexPath *idx in obj) {
                    invalidation_.invalidateDecoration(key, idx);
                }
            }];
        }
        if (ctx.invalidatedSectionScrollOffsets.count) {
            [ctx.invalidatedSectionScrollOffsets enumerateKeysAndObjectsUsingBlock:^(NSNumber * _Nonnull key, NSValue * _Nonnull obj, BOOL * _Nonnull stop) {
                invalidation_.invalidateSectionContentOffset(key.unsignedIntegerValue, obj.CGPointValue);
            }];
        }
    }
    
    void calculateLayoutIfNeeded() {
        NSUInteger minSection = NSNotFound;
        bool refreshSize = false;
        bool refreshSizeAll = false;
        if (!invalidation_.isInvalidateEverything() && invalidation_.isInvalidateDataSourceCounts()) {
            // Update actions(delete, insert) needs prev attributes. We cache it here ONLY update occurs.
            cacheResult();
        }
        else {
            clearCachedResult();
        }
        
        if (invalidation_.isInvalidateEverything()) {
            refreshAll();
            refreshSizeAll = refreshSize = true;
            minSection = 0;
        }
        else if (invalidation_.isInvalidateDataSourceCounts()) {
            if (!invalidateFlowLayoutDelegateMetricsWhenUpdates_ && updates_.hasModified()) {
                markUpdates();
                refreshSize = true;
                minSection = updates_.minSection();
            }
            else {
                refreshAll();
                refreshSizeAll = refreshSize = true;
                minSection = 0;
            }
        }
        else if (invalidation_.isInvalidateFlowLayoutDelegateMetrics()) {
            refreshAll();
            refreshSizeAll = refreshSize = true;
            minSection = 0;
        }
        if (!refreshSizeAll && invalidation_.isInvalidateFlowLayoutAttributes() && invalidation_.hasInvalidateAttributes()) {
            markInvalidation();
            refreshSize = true;
            minSection = std::min(minSection, invalidation_.minSection());
        }
        
        auto& offsets = invalidation_.invalidatedSectionContentOffsets();
        
        if (!refreshSize) {
            if (!offsets.empty()) {
                relayoutSectionContentOffset(offsets);
            }
        }
        else {
            updateContentOffsets(offsets);
            relayout(minSection == NSNotFound ? 0 : minSection);
        }
        
        invalidation_.reset();
        updates_.reset();
    }
    
    const auto& cachedPrevSections() { return cachedSections_; }
    
    void cacheResult() {
        cachedSections_ = sections_;
    }
    
    void clearCachedResult() {
        cachedSections_.clear();
    }
    
    #pragma mark - attributes
    void collectLayoutAttributesInRect(LayoutCollection& collection, const CGRect& rect, const CGRect& visibleRect) {
        for (auto& section : sections_) {
            section.collectLayoutAttributesInRect(context(), collection, rect, visibleRect);
        }
    }
    
    UICollectionViewLayoutAttributes *itemAttributedAtIndexPath(NSIndexPath *indexPath) {
        auto section = indexPath.section;
        if (section < sections_.size()) {
            return sections_.at(section).itemAttributeAtIndex(context_, indexPath.item);
        }
        return nil;
    }
    
    UICollectionViewLayoutAttributes *prevItemAttributedAtIndexPath(NSIndexPath *indexPath) {
        auto section = indexPath.section;
        if (section < cachedSections_.size()) {
            return cachedSections_.at(section).itemAttributeAtIndex(context_, indexPath.item);
        }
        return nil;
    }
    
    UICollectionViewLayoutAttributes *supplementaryAtIndexPath(NSString *kind, NSIndexPath *indexPath, const CGRect& visibleRect) {
        auto section = indexPath.section;
        if (section < sections_.size()) {
            return sections_.at(section).supplementaryAttributeAtIndex(context_, kind, indexPath.item, visibleRect);
        }
        return nil;
    }
    
    UICollectionViewLayoutAttributes *prevSupplementaryAtIndexPath(NSString *kind, NSIndexPath *indexPath, const CGRect& visibleRect) {
        auto section = indexPath.section;
        if (section < cachedSections_.size()) {
            return cachedSections_.at(section).supplementaryAttributeAtIndex(context_, kind, indexPath.item, visibleRect);
        }
        return nil;
    }
    
    UICollectionViewLayoutAttributes *prevDecorationAtIndexPath(NSString *kind, NSIndexPath *indexPath) {
        auto section = indexPath.section;
        if (section < cachedSections_.size()) {
            return cachedSections_.at(section).decorationAttributeAtIndex(context_, kind, indexPath.item);
        }
        return nil;
    }
    
    UICollectionViewLayoutAttributes *decorationAtIndexPath(NSString *kind, NSIndexPath *indexPath) {
        auto section = indexPath.section;
        if (section < sections_.size()) {
            return sections_.at(section).decorationAttributeAtIndex(context_, kind, indexPath.item);
        }
        return nil;
    }
    
    NSArray<NSIndexPath *> *sectionHeaderPinToVisibleIndexes() {
        NSMutableArray *array = [NSMutableArray array];
        for (auto& section : sections_) {
            if (section.headerPin() != PinToVisibleBounds::None
                && section.header().contentSize() != CGSizeZero) {
                [array addObject:[NSIndexPath indexPathForItem:0 inSection:section.section()]];
            }
        }
        return array;
    }
    NSArray<NSIndexPath *> *sectionFooterPinToVisibleIndexes() {
        NSMutableArray *array = [NSMutableArray array];
        for (auto& section : sections_) {
            if (section.footerPin() != PinToVisibleBounds::None
                && section.footer().contentSize() != CGSizeZero) {
                [array addObject:[NSIndexPath indexPathForItem:0 inSection:section.section()]];
            }
        }
        return array;
    }
    
    NSIndexPath *targetIndexPathAtPosition(const CGPoint& point) {
        for (auto& section : sections_) {
            if (CGRectContainsPoint(section.frame(), point)) {
                return section.targetIndexPathAtPosition(point);
            }
        }
        return nil;
    }
    
    CGSize contentSize() {
        return frame().size;
    }
protected:
    
    void markLayoutDirty() {
        for (auto& section : sections_) {
            section.markLayoutDirty();
        }
    }
    
    void markInvalidation() {
        for (auto& idx : invalidation_.invalidateItems()) {
            auto& section = sections_.at(idx.section());
            section.markItemDataSourceDirty(idx.item());
        }
        for (auto& [kind, idxes] : invalidation_.invalidateSumplementaries()) {
            for (auto& idx : idxes) {
                auto& section = sections_.at(idx.section());
                section.markSupplementaryDataSourceDirty(kind, idx.item());
            }
        }
        for (auto& [kind, idxes] : invalidation_.invalidateDecorations()) {
            for (auto& idx : idxes) {
                auto& section = sections_.at(idx.section());
                section.markDecorationDataSrouceDirty(kind, idx.item());
            }
        }
    }
        
    void markUpdates() {
        using namespace NE::CollectionView;
        for (auto& item : updates_.updateItems()) {
            switch (item.action()) {
                case UpdateAction::Insert:
                    if (item.isSection()) {
                        auto& sections = item.sections();
                        insertSections(sections);
                    }
                    else {
                        auto& indexPaths = item.indexPaths();
                        for (auto& indexPath : indexPaths) {
                            auto& section = sections_.at(indexPath.section());
                            section.insertItem(indexPath.item());
                        }
                    }
                    break;
                case UpdateAction::Delete:
                    if (item.isSection()) {
                        auto& sections = item.sections();
                        deleteSections(sections);
                    }
                    else {
                        auto& indexPaths = item.indexPaths();
                        for (auto& indexPath : indexPaths) {
                            auto& section = sections_.at(indexPath.section());
                            section.deleteItem(indexPath.item());
                        }
                    }
                    break;
                case UpdateAction::Reload:
                    if (item.isSection()) {
                        auto& sections = item.sections();
                        for (auto& section : sections) {
                            sections_.at(section).markDataSourceDirty();
                        }
                    }
                    else {
                        auto& items = item.indexPaths();
                        for (auto& indexPath : items) {
                            sections_.at(indexPath.section()).markItemDataSourceDirty(indexPath.item());
                        }
                    }
                    break;
                case UpdateAction::Move:
                    if (item.isSection()) {
                        auto fromIt = sections_.begin() + item.moveSections().first;
                        auto toIt = sections_.begin() + item.moveSections().second;
                        if (fromIt == toIt) {
                            return;
                        }
                        if (fromIt < toIt) {
                            fromIt->markLayoutDirty();
                            for (; fromIt != toIt; ++fromIt) {
                                std::iter_swap(fromIt, fromIt + 1);
                                fromIt->markLayoutDirty();
                            }
                        }
                        else {
                            fromIt->markLayoutDirty();
                            for (; fromIt != toIt; --fromIt) {
                                std::iter_swap(fromIt, fromIt - 1);
                                fromIt->markLayoutDirty();
                            }
                        }
                    }
                    else {
                        auto& from = item.moveIndexPaths().first;
                        auto& to = item.moveIndexPaths().second;
                        if (from.section() == to.section()) {
                            auto& section = sections_.at(from.section());
                            section.markLayoutDirty();
                        }
                        else {
                            auto& fromSection = sections_.at(from.section());
                            auto& toSection = sections_.at(to.section());
                            fromSection.deleteItem(from.item());
                            toSection.insertItem(to.item());
                        }
                    }
                    break;
            }
        }
    }
    
    void updateContentOffsets(const std::unordered_map<NSUInteger, CGPoint>& invalidatedOffsets) {
        if (invalidatedOffsets.empty()) return;
        for (auto& [index, offset] : invalidatedOffsets) {
            if (index < sections_.size()) {
                auto& section = sections_.at(index);
                section.setContentOffset(offset);
            }
        }
    }
    
    void refreshAll() {
        refreshDataSource();
        for (auto& section : sections_) {
            section.markDataSourceDirty();
        }
    }
    void refreshDataSource() {
        auto numberOfSections = context().numberOfSections();
        if (sections_.size() != numberOfSections) {
            sections_.resize(numberOfSections);
        }
    }
    void refreshIndexes() {
        NSUInteger idx = 0;
        for (auto& section : sections_) {
            section.setSection(idx);
            ++ idx;
        }
    }
    
    void relayoutSectionContentOffset(const std::unordered_map<NSUInteger, CGPoint>& invalidatedOffsets) {
        if (invalidatedOffsets.empty()) return;
        for (auto& [index, offset] : invalidatedOffsets) {
            if (index < sections_.size()) {
                auto& section = sections_.at(index);
                section.relayoutContentOffsetChanged(offset);
            }
        }
    }
    
    void relayout(NSInteger fromSection = 0) {
        refreshIndexes();
        auto origin = Node::origin();
        
        NSUInteger i = 0;
        auto sectionSpacing = context().sectionSpacing();
        if (context().sectionScrollDirection() == UICollectionViewScrollDirectionVertical) {
            auto contentSize = CGSize({
                .width = context().isCustomSectionWidth() ? context().sectionWidth() : bounds_.size.width,
                .height = 0
            });
            for (auto& section : sections_) {
                if (i >= fromSection) {
                    auto newOrigin = CGPoint({
                        .x = origin.x,
                        .y = origin.y + contentSize.height + (i != 0 ? sectionSpacing : 0)
                    });
                    if (newOrigin != section.origin()) {
                        section.setOrigin(newOrigin);
                        section.markLayoutDirty();
                    }
                    section.setFitSize(contentSize);
                    
                    section.relayout(context());
                }
                
                contentSize.height += section.frame().size.height;
                if (i > 0) {
                    contentSize.height += sectionSpacing;
                }
                ++i;
            }
            setSize(contentSize);
        }
        else {
            auto contentFitSize = CGSize({
                .width = context().isCustomSectionWidth() ? context().sectionWidth() : bounds_.size.width,
                .height = bounds_.size.height
            });
            auto contentSize = CGSize {
                .width = 0,
                .height = 0
            };
            for (auto& section : sections_) {
                if (i >= fromSection) {
                    auto newOrigin = CGPoint({
                        .x = origin.x + contentSize.width + (i != 0 ? sectionSpacing : 0),
                        .y = origin.y
                    });
                    if (newOrigin != section.origin()) {
                        section.setOrigin(newOrigin);
                        section.markLayoutDirty();
                    }
                    section.setFitSize(contentFitSize);
                    
                    section.relayout(context());
                }
                
                auto size = section.frame().size;
                contentSize.width += size.width;
                // height should use max section height?
                contentSize.height = std::max(contentSize.height, size.height);
                if (i > 0) {
                    contentSize.width += sectionSpacing;
                }
                
                ++i;
            }
            setSize(contentSize);
        }
    }
    
    void deleteSections(const std::set<NSUInteger>& idxes) {
        for (auto it = idxes.rbegin(); it != idxes.rend(); ++it) {
            deleteSection(*it);
        }
    }
    
    void deleteSection(NSUInteger idx) {
        sections_.erase(sections_.begin() + idx);
    }
    
    void insertSections(const std::set<NSUInteger>& idxes) {
        for (auto it = idxes.rbegin(); it != idxes.rend(); ++it) {
            insertSection(*it);
        }
    }
    void insertSection(NSUInteger idx) {
        sections_.emplace(sections_.begin() + idx);
    }
    
private:
    Context context_;
    Invalidation invalidation_;
    Updates updates_;
    CGRect bounds_;
    CGRect visibleRect_;
    std::vector<Section> sections_;
    std::vector<Section> cachedSections_;
    
    bool pinToVisibleBounds_ = false;
    bool invalidateFlowLayoutDelegateMetricsWhenUpdates_ = false;
    
    // calculate value
    bool dirty_ = true;
    bool scrollOffsetDirty_ = true;
    
    
    Calculator(Calculator&) = delete;
    Calculator& operator =(Calculator&) = delete;
};

}

#endif /* NECollectionViewFlowLayoutCalculator_h */
