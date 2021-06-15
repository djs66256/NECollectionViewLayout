//
//  NECollectionViewFlowLayoutItem.h
//  Pods
//
//  Created by Daniel on 2019/12/3.
//

#ifndef NECollectionViewFlowLayoutItem_h
#define NECollectionViewFlowLayoutItem_h

#import <UIKit/UIKit.h>
#include "NECollectionViewFlowLayoutNode.h"
#include "NECollectionViewLayoutHelpers.h"
#include "NECollectionViewFlowLayoutContext.h"

namespace NE::CollectionViewFlowLayout {
    
    constexpr NSInteger HeaderZIndex = 10;
    constexpr NSInteger FooterZIndex = 10;
    constexpr NSInteger ItemZIndex = 0;
    constexpr NSInteger BackgroundZIndex = -10;
    constexpr NSInteger ScrollContentZIndex = -10;

    struct ItemAttributesTraits {
        constexpr static NSInteger zIndex = ItemZIndex;
        static auto attributes(Context& ctx, NSIndexPath* indexPath) {
            return ctx.cellAttributes(indexPath);
        }
        static auto contentSize(Context& ctx, NSIndexPath* indexPath) {
            return ctx.sizeForItemAtIndexPath(indexPath);
        }
    };
    
    struct HeaderAttributesTraits {
        constexpr static NSInteger zIndex = HeaderZIndex;
        static auto attributes(Context& ctx, NSIndexPath* indexPath) {
            return ctx.supplementaryAttributes(UICollectionElementKindSectionHeader, indexPath);
        }
        static auto contentSize(Context& ctx, NSIndexPath* indexPath) {
            return ctx.referenceSizeForHeaderInSection(indexPath.section);
        }
    };
    
    struct FooterAttributesTraits {
        constexpr static NSInteger zIndex = FooterZIndex;
        static auto attributes(Context& ctx, NSIndexPath* indexPath) {
            return ctx.supplementaryAttributes(UICollectionElementKindSectionFooter, indexPath);
        }
        static auto contentSize(Context& ctx, NSIndexPath* indexPath) {
            return ctx.referenceSizeForFooterInSection(indexPath.section);
        }
    };
    
    extern "C" NSString * const NECollectionElementKindSectionBackground;
    struct BackgroundAttributesTraits {
        constexpr static NSInteger zIndex = BackgroundZIndex;
        static auto attributes(Context& ctx, NSIndexPath* indexPath) {
            return ctx.supplementaryAttributes(NECollectionElementKindSectionBackground, indexPath);
        }
        static auto contentSize(Context& ctx, NSIndexPath* indexPath) {
            return CGSizeZero;
        }
    };
    
    extern "C" NSString * const NECollectionElementKindSectionScrollContent;
    struct ScrollContentAttributesTraits {
        constexpr static NSInteger zIndex = ScrollContentZIndex;
        static auto attributes(Context& ctx, NSIndexPath* indexPath) {
            return ctx.decorationAttributes(NECollectionElementKindSectionScrollContent, indexPath);
        }
        static auto contentSize(Context& ctx, NSIndexPath* indexPath) {
            return CGSizeZero;
        }
    };
    
    template<typename Traits = ItemAttributesTraits>
    class AttributesContent : public Content {
    public:
        AttributesContent() : Content(), zIndex_(Traits::zIndex) {}
        AttributesContent(const CGPoint& origin) : Content(origin), zIndex_(Traits::zIndex) {}
        AttributesContent(const AttributesContent& other) : Content(other),
                                                            zIndex_(other.zIndex_),
                                                            dataSourceDirty_(other.dataSourceDirty_),
                                                            indexPath_(other.indexPath_) { };
        AttributesContent& operator=(const AttributesContent& other) {
            if (this == &other) return *this;
            Content::operator=(other);
            zIndex_ = other.zIndex_;
            dataSourceDirty_ = other.dataSourceDirty_;
            indexPath_ = other.indexPath_;
            return *this;
        };
        
        void setIndexPath(IndexPath indexPath) {
            indexPath_ = indexPath;
            attributes_ = nil;  // When indexPath changed, attributes MUST create a new one.
        }
        IndexPath indexPath() const { return { static_cast<NSUInteger>(indexPath_.section), static_cast<NSUInteger>(indexPath_.item) }; }
        
        void setZIndex(NSInteger zIndex) {
            zIndex_ = zIndex + Traits::zIndex;
        }
        NSInteger zIndex() const { return zIndex_; }
        
        template <bool force>
        void refreshSizeFromDelegate(Context& ctx) {
            if (force || dataSourceDirty()) {
                setContentSize(Traits::contentSize(ctx, indexPath_));
            }
            clearDataSourceDirty();
        }
        
        virtual NECollectionViewFlowLayoutAttributes* attributes(Context& ctx) {
            if (attributes_ == nil) {
                attributes_ = Traits::attributes(ctx, indexPath_);
            }
            attributes_.frame = frame();
            attributes_.zIndex = zIndex_;
            return attributes_;
        }

        void markDataSourceDirty() { dataSourceDirty_ = true; }
        void clearDataSourceDirty() { dataSourceDirty_ = false; }
        bool dataSourceDirty() { return dataSourceDirty_; }
        
    private:
        NSInteger zIndex_ = 0;
        bool dataSourceDirty_ = true;
        __strong NSIndexPath *indexPath_;
        __strong NECollectionViewFlowLayoutAttributes *attributes_;
    }; // END Item
    
    using Item = AttributesContent<ItemAttributesTraits>;
    using Header = AttributesContent<HeaderAttributesTraits>;
    using Footer = AttributesContent<FooterAttributesTraits>;
    using Background = AttributesContent<BackgroundAttributesTraits>;
    
    class ScrollContent : public AttributesContent<ScrollContentAttributesTraits> {
        using Super = AttributesContent<ScrollContentAttributesTraits>;
    public:
        using AttributesContent::AttributesContent;
        ScrollContent(const ScrollContent&) = default;
        ScrollContent& operator=(const ScrollContent&) = default;
        
        void setContentOffset(const CGPoint& contentOffset) {
            contentOffset_ = contentOffset;
        }
        const CGPoint& contentOffset() const {
            return contentOffset_;
        }
        
        void setContentSize(const CGSize& contentSize) {
            contentSize_ = contentSize;
        }
        const CGSize& contentSize() const {
            return contentSize_;
        }
        
        std::pair<bool, CGPoint> adjustContentOffsetToVisible() {
            if (pageEnable()) {
                auto pageWidth = pageSize_.width < 1 ? frame().size.width : pageSize_.width;
                auto index = static_cast<NSInteger>(round(contentOffset_.x / pageWidth));
                auto x = MAX(0, MIN(pageWidth * index, contentSize_.width - frame().size.width));
                return { contentOffset_.x != x, { x, contentOffset_.y } };
            }
            else {
                auto x = MAX(0, MIN(contentOffset_.x, contentSize_.width - frame().size.width));
                return { x != contentOffset_.x, { x, contentOffset_.y } };
            }
        }
        
        void setPageEnable(bool enable) { pageEnable_ = enable; }
        bool pageEnable() const { return pageEnable_; }
        
        void setPageSize(const CGSize size) { pageSize_ = size; }
        const CGSize& pageSize() const { return pageSize_; }
        
        NECollectionViewFlowLayoutAttributes* attributes(Context& ctx) override {
            auto attributes = Super::attributes(ctx);
            attributes.contentOffset = contentOffset();
            attributes.contentSize = contentSize();
            attributes.pageEnable = pageEnable_;
            attributes.pageSize = pageSize_;
            return attributes;
        }
        
    private:
        CGPoint contentOffset_ {0};
        CGSize contentSize_ {0};
        bool pageEnable_ = false;
        CGSize pageSize_ {0};
    };
}

#endif /* NECollectionViewFlowLayoutItem_h */
