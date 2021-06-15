//
//  NECollectionViewUpdaters.h
//  Pods
//
//  Created by Daniel on 2019/12/4.
//

#ifndef NECollectionViewUpdates_h
#define NECollectionViewUpdates_h

#import <UIKit/UIKit.h>
#include <vector>
#include <set>
#include <algorithm>
#include "NECollectionViewLayoutHelpers.h"

namespace NE::CollectionView {

    enum class UpdateAction {
        Insert,
        Delete,
        Reload,
        Move,
    };
    
    class UpdateItem {
    public:
        explicit UpdateItem(UpdateAction action, std::set<NSUInteger>&& section) :
        action_(action), isSection_(true), sections_(std::move(section))
        {}
        
        explicit UpdateItem(UpdateAction action, std::set<IndexPath>&& section) :
        action_(action), isSection_(false), indexPaths_(std::move(section))
        {}
        
        explicit UpdateItem(UpdateAction action, NSUInteger from, NSUInteger to) :
        action_(action), isSection_(true), moveSections_({from, to})
        {}
        
        explicit UpdateItem(UpdateAction action, IndexPath from, IndexPath to) :
        action_(action), isSection_(false), moveIndexPaths_({from, to})
        {}
        
        UpdateAction action() const { return action_; }
        bool isSection() const { return isSection_; }
        const auto& sections() const { return sections_; }
        const auto& indexPaths() const { return indexPaths_; }
        const auto& moveSections() const { return moveSections_; }
        const auto& moveIndexPaths() const { return moveIndexPaths_; }
        
    private:
        UpdateAction action_;
        bool isSection_;
        std::set<NSUInteger> sections_;
        std::set<IndexPath> indexPaths_;
        std::pair<NSUInteger, NSUInteger> moveSections_{0, 0};
        std::pair<IndexPath, IndexPath> moveIndexPaths_{0, 0};
    };
    
    class Updates {
    private:
        struct SectionCompare {
            bool operator()(const IndexPath& i1, const IndexPath& i2) {
                return i1.section() < i2.section();
            }
        };
    public:
        void insertSections(std::set<NSUInteger>&& section) {
            minSection_ = std::min(minSection_, *std::min_element(section.cbegin(), section.cend()));
            updates_.emplace_back(UpdateAction::Insert, std::move(section));
        }
        void deleteSections(std::set<NSUInteger>&& section) {
            minSection_ = std::min(minSection_, *std::min_element(section.cbegin(), section.cend()));
            updates_.emplace_back(UpdateAction::Delete, std::move(section));
        }
        
        void reloadSections(std::set<NSUInteger>&& section) {
            minSection_ = std::min(minSection_, *std::min_element(section.cbegin(), section.cend()));
            updates_.emplace_back(UpdateAction::Reload, std::move(section));
        }
        
        void moveSection(NSUInteger from, NSUInteger to) {
            minSection_ = std::min(minSection_, std::min(from, to));
            updates_.emplace_back(UpdateAction::Move, from, to);
        }
        
        void insertItems(std::set<IndexPath>&& indexPaths) {
            auto minIndexPath = indexPaths.begin();// std::min_element(indexPaths.cbegin(), indexPaths.cend(), SectionCompare());
            minSection_ = std::min(minSection_, minIndexPath->section());
            updates_.emplace_back(UpdateAction::Insert, std::move(indexPaths));
        }
        
        void deleteItems(std::set<IndexPath>&& indexPaths) {
            auto minIndexPath = indexPaths.begin();//std::min_element(indexPaths.cbegin(), indexPaths.cend(), SectionCompare());
            minSection_ = std::min(minSection_, minIndexPath->section());
            updates_.emplace_back(UpdateAction::Delete, std::move(indexPaths));
        }
        
        void reloadItems(std::set<IndexPath>&& indexPaths) {
            auto minIndexPath = indexPaths.begin();// std::min_element(indexPaths.cbegin(), indexPaths.cend(), SectionCompare());
            minSection_ = std::min(minSection_, minIndexPath->section());
            updates_.emplace_back(UpdateAction::Reload, std::move(indexPaths));
        }
        
        void moveItem(IndexPath from, IndexPath to) {
            minSection_ = std::min(minSection_, std::min(from.section(), to.section()));
            updates_.emplace_back(UpdateAction::Move, from, to);
        }
        
        NSUInteger minSection() const {
            return minSection_;
        }
        
        bool hasModified() const {
            return minSection_ != NSNotFound;
        }
        
        void reset() {
            minSection_ = NSNotFound;
            updates_.clear();
        }
        
        const std::vector<UpdateItem>& updateItems() const {
            return updates_;
        }
        
    private:
        std::vector<UpdateItem> updates_;
        NSUInteger minSection_{NSNotFound};
    };
    
}

#endif /* NECollectionViewUpdates_h */
