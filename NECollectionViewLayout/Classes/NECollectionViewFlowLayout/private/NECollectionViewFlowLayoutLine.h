//
//  NECollectionViewFlowLayoutLine.h
//  Pods
//
//  Created by Daniel on 2019/12/3.
//

#ifndef NECollectionViewFlowLayoutLine_h
#define NECollectionViewFlowLayoutLine_h

#include <vector>
#include "NECollectionViewFlowLayoutItem.h"

namespace NE::CollectionViewFlowLayout {
    
    class Line : public Container {
    public:
        Line(CGRect bounds, CGFloat spacing, Alignment alignHorizontal, Alignment alignVertical)
        : Container(bounds.origin, bounds.size), spacing_(spacing), alignHorizontal_(alignHorizontal), alignVertical_(alignVertical) {}
        
        
        bool pushItem(Item* item) {
            if (items_.size() == 0) {
                lineWidth_ = item->contentSize().width;
                lineHeight_ = item->contentSize().height;
                items_.push_back(item);
                return true;
            }
            else {
                auto newLineWidth = lineWidth_ + item->contentSize().width + spacing_;
                if (newLineWidth > fitSize().width) return false;
                
                lineWidth_ = newLineWidth;
                lineHeight_ = std::max(lineHeight_, item->contentSize().height);
                items_.push_back(item);
                return true;
            }
        }
        
        void calculateLayout() {
            if (items_.size() == 0) return ;
            if (items_.size() == 1) {
                //
                auto width = std::min(lineWidth_, fitSize().width);
                auto x = origin().x + CalculatePositionWithAlignment(alignHorizontal_, fitSize().width, width);
                auto item = items_[0];
                item->setFrame({
                    .origin = {
                        .x = x,
                        .y = origin().y,
                    },
                    .size = {
                        .width = width,
                        .height = item->contentSize().height
                    }
                });
            }
            else {
                auto origin = this->origin();
                origin.x += CalculatePositionWithAlignment(alignHorizontal_, fitSize().width, lineWidth_);
                auto spacing = spacing_;
                if (alignHorizontal_ == Alignment::SpacingBetween) {
                    auto itemsWidth = 0.;
                    for (auto item : items_) {
                        itemsWidth += item->contentSize().width;
                    }
                    spacing = (fitSize().width - itemsWidth) / (items_.size() - 1);
                }
                for (auto item : items_) {
                    item->setFrame({
                        .origin = {
                            .x = origin.x,
                            .y = origin.y + CalculatePositionWithAlignment(alignVertical_, lineHeight_, item->contentSize().height),
                        },
                        .size = item->contentSize()
                    });
                    origin.x += item->contentSize().width + spacing;
                }
            }
        }
        
        void newLine(CGFloat lineSpacing) {
            setOrigin({ origin().x, lineSpacing + origin().y + lineHeight_ });
            
            items_.clear();
            lineWidth_ = lineHeight_ = 0;
        }
        
        CGFloat lineWidth() { return lineWidth_; }
        CGFloat lineHeight() { return lineHeight_; }
        
        ~Line() override {}
    private:
        CGFloat spacing_{0};
        Alignment alignHorizontal_ = Alignment::Center;
        Alignment alignVertical_ = Alignment::Center;
        std::vector<Item *> items_;
        
        CGFloat lineWidth_{0};
        CGFloat lineHeight_{0};
        
        Line(Line&) = delete;
        Line& operator=(Line&) = delete;
    }; // END Line
    
    class Column : public Container {
    public:
        Column(CGRect bounds, CGFloat spacing, Alignment alignHorizontal, Alignment alignVertical)
        : Container(bounds.origin, bounds.size), spacing_(spacing), alignHorizontal_(alignHorizontal), alignVertical_(alignVertical) {}
        
        
        bool pushItem(Item* item) {
            if (items_.size() == 0) {
                columnWidth_ = item->contentSize().width;
                columnHeight_ = item->contentSize().height;
                items_.push_back(item);
                return true;
            }
            else {
                auto newColumnHeight = columnHeight_ + item->contentSize().height + spacing_;
                if (newColumnHeight > fitSize().height) return false;
                
                columnHeight_ = newColumnHeight;
                columnWidth_ = std::max(columnWidth_, item->contentSize().width);
                items_.push_back(item);
                return true;
            }
        }
        
        void calculateLayout() {
            if (items_.size() == 0) return ;
            if (items_.size() == 1) {
                //
                auto y = origin().y + CalculatePositionWithAlignment(alignVertical_, fitSize().height, columnHeight_);
                auto item = items_[0];
                item->setFrame({
                    .origin = {
                        .x = origin().x,
                        .y = y,
                    },
                    .size = item->contentSize()
                });
            }
            else {
                auto origin = this->origin();
                origin.y += CalculatePositionWithAlignment(alignVertical_, fitSize().height, columnHeight_);
                auto spacing = spacing_;
                if (alignVertical_ == Alignment::SpacingBetween) {
                    auto itemsHeight = 0.;
                    for (auto item : items_) {
                        itemsHeight += item->contentSize().height;
                    }
                    spacing = (fitSize().height - itemsHeight) / (items_.size() - 1);
                }
                for (auto item : items_) {
                    item->setFrame({
                        .origin = {
                            .x = origin.x + CalculatePositionWithAlignment(alignHorizontal_, columnWidth_, item->contentSize().width),
                            .y = origin.y,
                        },
                        .size = item->contentSize()
                    });
                    origin.y += item->contentSize().height + spacing;
                }
            }
        }
        
        void newColumn(CGFloat lineSpacing) {
            setOrigin({ origin().x + lineSpacing + columnWidth_, origin().y});
            
            items_.clear();
            columnWidth_ = columnHeight_ = 0;
        }
        
        CGFloat columnWidth() { return columnWidth_; }
        CGFloat columnHeight() { return columnHeight_; }
        
        ~Column() override {}
    private:
        CGFloat spacing_{0};
        Alignment alignHorizontal_ = Alignment::Center;
        Alignment alignVertical_ = Alignment::Center;
        std::vector<Item *> items_;
        
        CGFloat columnWidth_{0};
        CGFloat columnHeight_{0};
        
        Column(Column&) = delete;
        Column& operator=(Column&) = delete;
    };
    
}

#endif /* NECollectionViewFlowLayoutLine_h */
