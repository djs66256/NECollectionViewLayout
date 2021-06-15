//
//  NECollectionViewFlowLayoutNode.h
//  Pods
//
//  Created by Daniel on 2019/12/3.
//

#ifndef NECollectionViewFlowLayoutNode_h
#define NECollectionViewFlowLayoutNode_h

#import <UIKit/UIKit.h>

namespace NE::CollectionViewFlowLayout {
    class Node {
    public:
        Node() = default;
        Node(const CGPoint& origin) : frame_({origin, CGSizeZero}) {}
        Node(const Node&) = default;
        Node& operator=(const Node&) = default;
        virtual ~Node() = default;
        
        void setOrigin(const CGPoint& origin) { frame_.origin =  origin; }
        const CGPoint& origin() const { return frame_.origin; }
        
        void setSize(const CGSize& size) { frame_.size = size; }
        const CGSize& size() const { return frame_.size; }
        
        void setFrame(const CGRect& frame) { frame_ = frame; }
        const CGRect& frame() const { return frame_; }
        
    protected:
        CGRect frame_{0};
    };

    class Container : public Node {
    public:
        using Node::Node;
        Container(CGPoint origin, CGSize fitSize) : Node(origin), fitSize_(fitSize) {}
        Container(const Container&) = default;
        Container& operator=(const Container&) = default;
        
        void setFitSize(CGSize fitSize) { fitSize_ = fitSize; }
        const CGSize& fitSize() const { return fitSize_; }
        
    protected:
        CGSize fitSize_{0};
        
    };

    class Content : public Node {
    public:
        using Node::Node;
        Content(const Content&) = default;
        Content& operator=(const Content&) = default;
        
        void setContentSize(CGSize size) { contentSize_ = size; }
        const CGSize& contentSize() const { return contentSize_; }
        
    protected:
        CGSize contentSize_{0};
    };
}

#endif /* NECollectionViewFlowLayoutNode_h */
