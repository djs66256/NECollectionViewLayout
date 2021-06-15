//
//  NECollectionViewFlowLayoutCollection.h
//  Pods
//
//  Created by Daniel on 2019/12/4.
//

#ifndef NECollectionViewFlowLayoutCollection_h
#define NECollectionViewFlowLayoutCollection_h

#import <UIKit/UIKit.h>

namespace NE::CollectionViewFlowLayout {

class LayoutCollection {
public:
    LayoutCollection() {
        attributes_ = [NSMutableArray arrayWithCapacity:0];
    }
    
    void addItem(UICollectionViewLayoutAttributes *attr) {
        [attributes_ addObject:attr];
    }
    
    void addSupplementary(UICollectionViewLayoutAttributes *attr) {
        [attributes_ addObject:attr];
    }
    
    void addDecoration(UICollectionViewLayoutAttributes *attr) {
        [attributes_ addObject:attr];
    }
    
    NSMutableArray<UICollectionViewLayoutAttributes *> *attributes() {
        return attributes_;
    }
    
private:
    __strong NSMutableArray<UICollectionViewLayoutAttributes *> *attributes_ = nil;
};
    
}

#endif /* NECollectionViewFlowLayoutCollection_h */
