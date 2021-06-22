//
//  NECollectionViewFlowLayoutSection.h
//  Pods
//
//  Created by Daniel on 2019/12/3.
//

#ifndef NECollectionViewFlowLayoutSection_h
#define NECollectionViewFlowLayoutSection_h

#include <vector>
#include <iostream>
#include "NECollectionViewFlowLayoutItem.h"
#include "NECollectionViewFlowLayoutLine.h"
#include "NECollectionViewFlowLayoutNode.h"
#include "NECollectionViewFlowLayoutContext.h"
#include "NECollectionViewLayoutHelpers.h"
#include "NECollectionViewFlowLayoutTypes.h"
#include "NECollectionViewFlowLayoutCollection.h"

namespace NE::CollectionViewFlowLayout {

class Section : public Container {
public:
#define Property(Type, name, Name)  \
void set ## Name(Type& name) {      \
    if (name ## _ != name) {        \
        name ## _ = name;           \
        markLayoutDirty();          \
    }                               \
}                                   \
const Type& name() const { return name ## _; }

    Property(CGFloat, itemSpacing, ItemSpacing)
    Property(CGFloat, lineSpacing, LineSpacing)
    Property(UIEdgeInsets, insets, Insets)
    Property(Alignment, alignHorizontal, AlignHorizontal)
    Property(Alignment, alignVertical, AlignVertical)
    Property(NSInteger, additionZIndex, AdditionZIndex)
    
#undef Property
    
#define Dirty(name, Name)   \
void mark ## Name() { name ## _ = true; } \
bool name() const { return name ## _; } \
void clear ## Name() { name ## _ = false; }
    
    Dirty(layoutDirty, LayoutDirty)
    Dirty(dataSourceDirty, DataSourceDirty)
    Dirty(itemDataSourceDirty, ItemDataSourceDirty)
    Dirty(indexDirty, IndexDirty)
    
#undef Dirty

    using Container::Container;
    Section(const Section&) = default;
    Section& operator=(const Section&) = default;
    
    #pragma mark - Layout
    
    void relayout(Context& ctx) {
        bool forceLoad = false;
        if (dataSourceDirty()) {
            forceLoad = true;
            refreshDataSource(ctx);
            clearDataSourceDirty();
            
            markIndexDirty();
            markItemDataSourceDirty();
            markLayoutDirty();
        }
        
        if (indexDirty()) {
            refreshIndexes();
            clearIndexDirty();
        }
        
        if (itemDataSourceDirty()) {
            if (forceLoad) {
                refreshSizeFromDelegate<true>(ctx);
            }
            else {
                refreshSizeFromDelegate<false>(ctx);
            }
            clearItemDataSourceDirty();
        }
        
        if (layoutDirty()) {
            relayoutForce(ctx);
            clearLayoutDirty();
        }
    }
    
    void relayoutContentOffsetChanged(const CGPoint& offset) {
        assert(!layoutDirty());
        updateContentOffset(offset);
    }
    
    #pragma mark - Layout attributes
    void collectLayoutAttributesInRect(Context& ctx, LayoutCollection& collection, const CGRect& rect, const CGRect& visibleRect) {
        auto frame = this->frame();
        if (auto header = headerAttributeInVisibleRect(ctx, rect, visibleRect)) {
            collection.addSupplementary(header);
        }
        if (auto footer = footerAttributeInVisibleRect(ctx, rect, visibleRect)) {
            collection.addSupplementary(footer);
        }
        if (CGRectIntersectsRect(frame, rect)) {
            for (auto& item : items_) {
                if (CGRectIntersectsRect(item.frame(), rect)) {
                    collection.addItem(item.attributes(ctx));
                }
            }
            if (backgroundVisible_) {
                collection.addSupplementary(background_.attributes(ctx));
            }
            if (scrollDirection_ == ScrollDirection::Horizontal) {
                collection.addDecoration(scrollContent_.attributes(ctx));
            }
        }
    }
    
    UICollectionViewLayoutAttributes *itemAttributeAtIndex(Context& ctx, NSUInteger index) {
        if (index < items_.size()) {
            return items_.at(index).attributes(ctx);
        }
        return nil;
    }
    UICollectionViewLayoutAttributes *supplementaryAttributeAtIndex(Context& ctx, NSString *kind, NSUInteger index, const CGRect& visibleRect) {
        if ([kind isEqualToString:UICollectionElementKindSectionHeader]) {
            return headerAttributeInVisibleRect(ctx, CGRectMake(0, 0, CGFLOAT_MAX, CGFLOAT_MAX), visibleRect);
        }
        else if ([kind isEqualToString:UICollectionElementKindSectionFooter]) {
            return footerAttributeInVisibleRect(ctx, CGRectMake(0, 0, CGFLOAT_MAX, CGFLOAT_MAX), visibleRect);
        }
        else if (backgroundVisible_ && [kind isEqualToString:NECollectionElementKindSectionBackground]) {
            return background_.attributes(ctx);
        }
        return nil;
    }
    UICollectionViewLayoutAttributes *decorationAttributeAtIndex(Context& ctx, NSString *kind, NSUInteger index) {
        if ([kind isEqualToString:NECollectionElementKindSectionScrollContent]) {
            return scrollContent_.attributes(ctx);
        }
        return nil;
    }
    UICollectionViewLayoutAttributes *headerAttributeInVisibleRect(Context& ctx, const CGRect& rect, const CGRect& visibleRect);
    UICollectionViewLayoutAttributes *footerAttributeInVisibleRect(Context& ctx, const CGRect& rect, const CGRect& visibleRect);
    NSIndexPath *targetIndexPathAtPosition(const CGPoint& point) {
        for (auto& item : items_) {
            if (CGRectContainsPoint(item.frame(), point)) {
                return item.indexPath();
            }
        }
        return nil;
    }
    
    #pragma mark - Edit
    void insertItem(NSUInteger idx) {
        items_.emplace(items_.begin() + idx);
        markItemDataSourceDirty();
        markLayoutDirty();
        markIndexDirty();
    }
    void deleteItem(NSUInteger idx) {
        items_.erase(items_.begin() + idx);
        markItemDataSourceDirty();
        markLayoutDirty();
        markIndexDirty();
    }
    void move(NSUInteger from, NSUInteger to) {
        auto fromIt = items_.begin() + from;
        auto toIt = items_.begin() + to;
        if (fromIt == toIt) {
            return;
        }
        else if (fromIt < toIt) {
            for (; fromIt != toIt; ++fromIt) {
                std::iter_swap(fromIt, fromIt + 1);
            }
        }
        else {
            for (; fromIt != toIt; --fromIt) {
                std::iter_swap(fromIt, fromIt - 1);
            }
        }
        markLayoutDirty();
        markIndexDirty();
    }
    
    #pragma mark - dirty
    
    void markItemDataSourceDirty(NSUInteger item) {
        items_.at(item).markDataSourceDirty();
        markItemDataSourceDirty();
        markLayoutDirty();
    }
    void markSupplementaryDataSourceDirty(NSString *kind, NSUInteger idx) {
        if ([kind isEqualToString:UICollectionElementKindSectionHeader]) {
            header_.markDataSourceDirty();
        }
        else if ([kind isEqualToString:UICollectionElementKindSectionFooter]) {
            footer_.markDataSourceDirty();
        }
        markItemDataSourceDirty();
        markLayoutDirty();
    }
    void markDecorationDataSrouceDirty(NSString *kind, NSUInteger idx) {
        
    }
    
    #pragma mark - getter & setter
    void setSection(NSUInteger section) {
        if (section_ != section) {
            section_ = section;
            markIndexDirty();
        }
    }
    NSUInteger section() const { return section_; }
    
    auto itemCount() const { return items_.size(); }
    
    void setContentOffset(CGPoint offset) {
        scrollContent_.setContentOffset(offset);
        markLayoutDirty();
    }
    const CGPoint& contentOffset() const { return scrollContent_.contentOffset(); }
    
    const Header& header() const { return header_; }
    const Footer& footer() const { return footer_; }
    
    PinToVisibleBounds headerPin() { return headerPin_; }
    PinToVisibleBounds footerPin() { return footerPin_; }
    
private:
    // section properties
    CGFloat itemSpacing_{0};
    CGFloat lineSpacing_{0};
    UIEdgeInsets insets_{0};
    
    NSInteger additionZIndex_ = 0;
    
    Alignment alignHorizontal_ = Alignment::Center;
    Alignment alignVertical_ = Alignment::Center;
    
    NSUInteger section_ = 0;
    std::vector<Item> items_;
    
    Header header_;
    Footer footer_;

    // size dirty
    bool layoutDirty_ = true;           // need relayout
    bool dataSourceDirty_ = true;       // need refresh numberOfSection
    bool itemDataSourceDirty_ = true;   // need refresh sizeOfItem
    bool indexDirty_ = true;
    
    // background
    bool backgroundVisible_ = false;
    bool backgroundIncludeSumpplementary_ = false;
    UIEdgeInsets backgroundInsets_ {0};
    Background background_;
    
    // scroll horizontal
    ScrollDirection scrollDirection_ = ScrollDirection::Vertical;
    ScrollContent scrollContent_;
    CGFloat heightForScrollHorizontal_ = 0;
    
    // pin to visible bounds
    PinToVisibleBounds headerPin_;
    PinToVisibleBounds footerPin_;
    
    
    void updateContentOffset(const CGPoint& offset) {
        CGPoint originOffset = contentOffset();
        auto deltaX = offset.x - originOffset.x;
        if (fabs(deltaX) < 0.1) return ;
        for (auto& item : items_) {
            item.setOrigin({
                item.origin().x - deltaX,
                item.origin().y
            });
        }
        scrollContent_.setContentOffset(offset);
    }
    
    void refreshIndexes() {
        auto section = this->section();
        NSUInteger idx = 0;
        for (auto& item : items_) {
            item.setIndexPath({section, idx});
            item.setZIndex(additionZIndex_);
            ++ idx;
        }
        background_.setIndexPath({section, 0});
        background_.setZIndex(additionZIndex_);
        scrollContent_.setIndexPath({section, 0});
        scrollContent_.setZIndex(additionZIndex_);
        
        header_.setIndexPath({ section, 0 });
        header_.setZIndex(additionZIndex_);
        footer_.setIndexPath({ section, 0 });
        footer_.setZIndex(additionZIndex_);
        
        clearIndexDirty();
    }
    void refreshDataSource(Context& ctx);
    template<bool force>
    void refreshSizeFromDelegate(Context& ctx);
    void relayoutForce(Context& ctx);
}; // END Section
    
    inline void Section::refreshDataSource(Context& ctx) {
        auto section = this->section();
        itemSpacing_ = ctx.minimumInteritemSpacingForSectionAtIndex(section);
        lineSpacing_ = ctx.minimumLineSpacingForSectionAtIndex(section);
        insets_ = ctx.insetForSectionAtIndex(section);
        additionZIndex_ = ctx.additionZIndexForSectionAtIndex(section);
        alignHorizontal_ = ctx.alignHorizontalForSectionAtIndex(section);
        alignVertical_ = ctx.alignVerticalForSectionAtIndex(section);
        backgroundVisible_ = ctx.backgroundVisibleForSectionAtIndex(section);
        if (backgroundVisible_) {
            backgroundIncludeSumpplementary_ = ctx.backgroundIncludeSupplementarysForSectionAtIndex(section);
            backgroundInsets_ = ctx.backgroundInsetsForSectionAtIndex(section);
        }
        scrollDirection_ = ctx.scrollDirectionForSectionAtIndex(section);
        if (scrollDirection_ == ScrollDirection::Horizontal) {
            scrollContent_.setPageEnable(ctx.pageEnableForSectionAtIndex(section));
            scrollContent_.setPageSize(ctx.pageSizeForSectionAtIndex(section));
            heightForScrollHorizontal_ = ctx.heightForScrollHorizontalSectionAtIndex(section);
        }
        auto numberOfItems = ctx.numberOfItemsInSection(section);
        if (items_.size() != numberOfItems) {
            items_.resize(numberOfItems);
        }
    }
    
    template<bool force>
    void Section::refreshSizeFromDelegate(Context& ctx) {
        auto section = this->section();
        NSUInteger index = 0;
        for (auto& item : items_) {
            item.refreshSizeFromDelegate<force>(ctx);
            
            ++index;
        }
        
        headerPin_ = ctx.headerPinToVisibleBoundsForSectionAtIndex(section);
        header_.refreshSizeFromDelegate<force>(ctx);
        
        footerPin_ = ctx.footerPinToVisibleBoundsForSectionAtIndex(section);
        footer_.refreshSizeFromDelegate<force>(ctx);
    }
    
    inline void Section::relayoutForce(Context& ctx) {
        CGRect bounds = {
            .origin = origin(),
            .size = {
                .width = fitSize().width,
                .height = CGFLOAT_MAX,
            }
        };
        
        if (header_.contentSize() != CGSizeZero) {
            auto width = std::min(header_.contentSize().width, fitSize().width);
            auto x = CalculatePositionWithAlignment(alignHorizontal(), fitSize().width, width);
            header_.setFrame({
                .origin = {
                    .x = bounds.origin.x + x,
                    .y = bounds.origin.y
                },
                .size = {
                    .width = width,
                    .height = header_.contentSize().height
                }
            });
            
            bounds.origin.y += header_.size().height;
        }
        
        CGRect itemBounds = bounds + insets();
        itemBounds.origin.x -= contentOffset().x;
        // We give it a little more space, because of the precision
        itemBounds.size.width += 0.1;
        itemBounds.size.height += 0.1;
        bounds.origin.y += insets().top;
        auto alignHorizontal = this->alignHorizontal();
        bool useLineLayout = true;
        if (scrollDirection_ == ScrollDirection::Horizontal) {
            // When scoll direction is horizontal, exist only one line.
            itemBounds.size.width = CGFLOAT_MAX;
            itemBounds.size.height = heightForScrollHorizontal_ != 0 ? heightForScrollHorizontal_ : CGFLOAT_MAX;
            // And this line MUST be align leading, as well as left.
            alignHorizontal = Alignment::Leading;
            useLineLayout = heightForScrollHorizontal_ == 0;
        }
        if (useLineLayout) {
            Line line(itemBounds, itemSpacing(), alignHorizontal, alignVertical());
            for (auto& item : items_) {
                if (line.pushItem(&item)) {
                    // add item to a line.
                }
                else {
                    line.calculateLayout();
                    // begin a new line.
                    bounds.origin.y += line.lineHeight() + lineSpacing();
                    line.newLine(lineSpacing());
                    line.pushItem(&item);
                }
            }
            line.calculateLayout();
            if (scrollDirection_ == ScrollDirection::Horizontal) {
                scrollContent_.setFrame({
                    { bounds.origin.x, bounds.origin.y - insets().top },
                    { bounds.size.width, line.lineHeight() + insets().top + insets().bottom }
                });
                scrollContent_.setContentSize(CGSize({line.lineWidth(), line.lineHeight()}) - insets());
                auto result = scrollContent_.adjustContentOffsetToVisible();
                if (result.first) {
                    updateContentOffset(result.second);
                }
            }
            bounds.origin.y += line.lineHeight();
        }
        else {
            CGFloat contentWidth = 0.;
            Column line(itemBounds, itemSpacing(), alignHorizontal, alignVertical());
            for (auto& item : items_) {
                if (line.pushItem(&item)) {
                    // add item to a line.
                }
                else {
                    line.calculateLayout();
                    // begin a new line.
                    contentWidth += line.columnWidth() + lineSpacing();
                    line.newColumn(lineSpacing());
                    line.pushItem(&item);
                }
            }
            line.calculateLayout();
            contentWidth += line.columnWidth();
            scrollContent_.setFrame({
                { bounds.origin.x, bounds.origin.y - insets().top },
                { bounds.size.width, heightForScrollHorizontal_ + insets().top + insets().bottom }
            });
            scrollContent_.setContentSize(CGSize({contentWidth, heightForScrollHorizontal_}) - insets());
            auto result = scrollContent_.adjustContentOffsetToVisible();
            if (result.first) {
                updateContentOffset(result.second);
            }
            
            bounds.origin.y += heightForScrollHorizontal_;
        }
        bounds.origin.y += insets().bottom;
        
        if (footer_.contentSize() != CGSizeZero) {
            auto width = std::min(footer_.contentSize().width, fitSize().width);
            auto x = CalculatePositionWithAlignment(this->alignHorizontal(), fitSize().width, width);
            footer_.setFrame({
                .origin = {
                    .x = bounds.origin.x + x,
                    .y = bounds.origin.y
                },
                .size = {
                    .width = width,
                    .height = footer_.contentSize().height
                }
            });
            
            bounds.origin.y += footer_.size().height;
        }
        
        setSize({
            .width = fitSize().width,
            .height = bounds.origin.y - origin().y,
        });
        
        if (backgroundVisible_) {
            if (backgroundIncludeSumpplementary_) {
                auto frame = CGRect({
                    origin() + backgroundInsets_,
                    size() + backgroundInsets_
                });
                background_.setFrame(frame);
            }
            else {
                auto frame = CGRect({
                    .origin = CGPoint({
                        origin().x,
                        origin().y + header_.size().height
                    }) + insets() + backgroundInsets_,
                    .size = CGSize({
                        size().width,
                        size().height - header_.size().height - footer_.size().height
                    }) + insets() + backgroundInsets_
                });
                background_.setFrame(frame);
            }
        }
    }
    inline UICollectionViewLayoutAttributes *Section::headerAttributeInVisibleRect(Context& ctx, const CGRect& rect, const CGRect& visibleRect) {
        if (header_.contentSize() != CGSizeZero) {
            switch (headerPin()) {
                case PinToVisibleBounds::None:
                    if (CGRectIntersectsRect(header_.frame(), rect)) {
                        return header_.attributes(ctx);
                    }
                    break;
                case PinToVisibleBounds::InsideSection:
                    if (header_.frame().origin.y < visibleRect.origin.y) {
                        NECollectionViewFlowLayoutAttributes *header = header_.attributes(ctx).copy;
                        CGRect frame = header_.frame();
                        frame.origin.y = std::min(visibleRect.origin.y, CGRectGetMaxY(this->frame()) - footer_.size().height - header_.size().height);
                        header.frame = frame;
                        header.pinned = true;
                        return header;
                    }
                    else if (CGRectIntersectsRect(header_.frame(), rect)) {
                        return header_.attributes(ctx);
                    }
                    break;
                case PinToVisibleBounds::Always: {
                    NECollectionViewFlowLayoutAttributes *header = header_.attributes(ctx).copy;
                    auto frame = header_.frame();
                    frame.origin.y = visibleRect.origin.y;
                    header.frame = frame;
                    header.zIndex ++;
                    header.pinned = true;
                    return header;
                }
                    break;
                case PinToVisibleBounds::AfterSection:
                    if (header_.frame().origin.y < visibleRect.origin.y) {
                        NECollectionViewFlowLayoutAttributes *header = header_.attributes(ctx).copy;
                        auto frame = header_.frame();
                        frame.origin.y = visibleRect.origin.y;
                        header.frame = frame;
                        header.zIndex ++;
                        header.pinned = true;
                        return header;
                    }
                    else if (CGRectIntersectsRect(header_.frame(), rect)) {
                        return header_.attributes(ctx);
                    }
                    break;
                case PinToVisibleBounds::BeforeSection:
                    if (header_.frame().origin.y > visibleRect.origin.y) {
                        NECollectionViewFlowLayoutAttributes *header = header_.attributes(ctx).copy;
                        auto frame = header_.frame();
                        frame.origin.y = visibleRect.origin.y;
                        header.frame = frame;
                        header.zIndex ++;
                        header.pinned = true;
                        return header;
                    }
                    else if (CGRectIntersectsRect(header_.frame(), rect)) {
                        return header_.attributes(ctx);
                    }
                    break;
                default:
                    break;
            }
        }
        return nil;
    }
    inline UICollectionViewLayoutAttributes *Section::footerAttributeInVisibleRect(Context& ctx, const CGRect& rect, const CGRect& visibleRect) {
        if (footer_.contentSize() != CGSizeZero) {
            switch (footerPin()) {
                case PinToVisibleBounds::None:
                    if (CGRectIntersectsRect(footer_.frame(), rect)) {
                        return footer_.attributes(ctx);
                    }
                    break;
                case PinToVisibleBounds::InsideSection:
                    if (CGRectGetMaxY(footer_.frame()) > CGRectGetMaxY(visibleRect)) {
                        NECollectionViewFlowLayoutAttributes *footer = footer_.attributes(ctx).copy;
                        auto frame = footer_.frame();
                        frame.origin.y = std::max(CGRectGetMaxY(visibleRect) - footer_.size().height, this->origin().y + header_.size().height);
                        footer.frame = frame;
                        footer.pinned = true;
                        return footer;
                    }
                    else if (CGRectIntersectsRect(footer_.frame(), rect)) {
                        return footer_.attributes(ctx);
                    }
                    break;
                case PinToVisibleBounds::Always: {
                    NECollectionViewFlowLayoutAttributes *footer = footer_.attributes(ctx).copy;
                    auto frame = footer_.frame();
                    frame.origin.y = CGRectGetMaxY(visibleRect) - footer_.size().height;
                    footer.frame = frame;
                    footer.zIndex ++;
                    footer.pinned = true;
                    return footer;
                }
                    break;
                case PinToVisibleBounds::AfterSection:
                    if (CGRectGetMaxY(footer_.frame()) < CGRectGetMaxY(visibleRect)) {
                        NECollectionViewFlowLayoutAttributes *footer = footer_.attributes(ctx).copy;
                        auto frame = footer_.frame();
                        frame.origin.y = CGRectGetMaxY(visibleRect) - footer_.size().height;
                        footer.frame = frame;
                        footer.zIndex ++;
                        footer.pinned = true;
                        return footer;
                    }
                    else if (CGRectIntersectsRect(header_.frame(), rect)) {
                        return footer_.attributes(ctx);
                    }
                    break;
                case PinToVisibleBounds::BeforeSection:
                    if (CGRectGetMaxY(footer_.frame()) > CGRectGetMaxY(visibleRect)) {
                        NECollectionViewFlowLayoutAttributes *footer = footer_.attributes(ctx).copy;
                        auto frame = footer_.frame();
                        frame.origin.y = CGRectGetMaxY(visibleRect) - footer_.size().height;
                        footer.frame = frame;
                        footer.zIndex ++;
                        footer.pinned = true;
                        return footer;
                    }
                    else if (CGRectIntersectsRect(footer_.frame(), rect)) {
                        return footer_.attributes(ctx);
                    }
                    break;
                default:
                    break;
            }
        }
        return nil;
    }
}

#endif /* NECollectionViewFlowLayoutSection_h */
