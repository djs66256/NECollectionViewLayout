//
//  NECollectionViewFlowLayoutInvalidation.h
//  Pods
//
//  Created by Daniel on 2019/12/3.
//

#ifndef NECollectionViewFlowLayoutInvalidation_h
#define NECollectionViewFlowLayoutInvalidation_h

#include <set>
#include <unordered_map>
#include <array>
#import <UIKit/UIKit.h>
#include "NECollectionViewLayoutHelpers.h"

namespace NE::CollectionViewFlowLayout {

    class Invalidation {
    public:
        
        void invalidateEverything() { invalidateEverything_ = true; }
        bool isInvalidateEverything() { return invalidateEverything_; }
        void invalidateDataSourceCounts() { invalidateDataSourceCounts_ = true; }
        bool isInvalidateDataSourceCounts() { return invalidateDataSourceCounts_; }
        void invalidateFlowLayoutDelegateMetrics() { invalidateFlowLayoutDelegateMetrics_ = true; }
        bool isInvalidateFlowLayoutDelegateMetrics() { return invalidateFlowLayoutDelegateMetrics_; }
        void invalidateFlowLayoutAttributes() { invalidateFlowLayoutAttributes_ = true; }
        bool isInvalidateFlowLayoutAttributes() { return invalidateFlowLayoutAttributes_; }
        
        void invalidateItem(const IndexPath indexPath) {
            minSection_ = std::min(minSection_, indexPath.section());
            invalidateItems_.insert(indexPath);
        }
        
        const auto& invalidateItems() { return invalidateItems_; }
        
        void invalidateSumplementary(NSString *kind, const IndexPath indexPath) {
            minSection_ = std::min(minSection_, indexPath.section());
            auto& idxes = invalidateSumplementaries_[kind];
            idxes.push_back(indexPath);
        }
        
        const auto& invalidateSumplementaries() const { return invalidateSumplementaries_; }
        
        void invalidateDecoration(NSString *kind, const IndexPath indexPath) {
            minSection_ = std::min(minSection_, indexPath.section());
            auto& idxes = invalidateDecorations_[kind];
            idxes.push_back(indexPath);
        }
        
        const auto& invalidateDecorations() const { return invalidateDecorations_; }
        
        void invalidateSectionContentOffset(const NSUInteger index, CGPoint contentOffset) {
            minSection_ = std::min(minSection_, index);
            invalidateScrollOffsets_[index] = contentOffset;
        }
        const auto& invalidatedSectionContentOffsets() const { return invalidateScrollOffsets_; }
        
        NSUInteger minSection() {
            return minSection_;
        }
        
        bool hasInvalidateAttributes() {
            return minSection_ != NSNotFound;
        }
        
        void reset() {
            invalidateEverything_ = false;
            invalidateDataSourceCounts_ = false;
            invalidateFlowLayoutDelegateMetrics_ = false;
            invalidateFlowLayoutAttributes_ = false;
            invalidateItems_.clear();
            invalidateSumplementaries_.clear();
            invalidateDecorations_.clear();
            invalidateScrollOffsets_.clear();
            minSection_ = NSNotFound;
        }
    private:
        bool invalidateEverything_{false};
        bool invalidateDataSourceCounts_{false};
        bool invalidateFlowLayoutDelegateMetrics_{false};
        bool invalidateFlowLayoutAttributes_{false};
        NSUInteger minSection_{NSNotFound};
        
        std::set<IndexPath> invalidateItems_;
        std::unordered_map<ObjcRef<NSString>, std::vector<IndexPath>> invalidateSumplementaries_;
        std::unordered_map<ObjcRef<NSString>, std::vector<IndexPath>> invalidateDecorations_;
        std::unordered_map<NSUInteger, CGPoint> invalidateScrollOffsets_;
    };
    
}

#endif /* NECollectionViewFlowLayoutInvalidation_h */
