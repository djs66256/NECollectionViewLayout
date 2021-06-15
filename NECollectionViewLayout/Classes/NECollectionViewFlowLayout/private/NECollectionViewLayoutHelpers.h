//
//  NECollectionViewLayoutHelpers.h
//  Pods
//
//  Created by Daniel on 2019/12/3.
//

#ifndef NECollectionViewLayoutHelpers_h
#define NECollectionViewLayoutHelpers_h

#include <UIKit/UIKit.h>
#include <array>

namespace NE {

inline CGPoint operator+ (const CGPoint& left, const UIEdgeInsets& insets) {
    return CGPoint({
        .x = left.x + insets.left,
        .y = left.y + insets.top,
    });
}
inline CGPoint operator- (const CGPoint& left, const UIEdgeInsets& insets) {
    return CGPoint({
        .x = left.x - insets.left,
        .y = left.y - insets.top,
    });
}
inline CGSize operator+ (const CGSize& left, const UIEdgeInsets& insets) {
    return CGSize({
        .width = left.width - insets.left - insets.right,
        .height = left.height - insets.top - insets.bottom
    });
}
inline CGSize operator- (const CGSize& left, const UIEdgeInsets& insets) {
    return CGSize({
        .width = left.width + insets.left + insets.right,
        .height = left.height + insets.top + insets.bottom
    });
}

inline CGRect operator+ (const CGRect& left, const UIEdgeInsets& insets) {
    return {
        .origin = left.origin + insets,
        .size = left.size + insets
    };
}
inline CGRect operator- (const CGRect& left, const UIEdgeInsets& insets) {
    return {
        .origin = left.origin - insets,
        .size = left.size - insets
    };
}

inline bool operator== (const CGRect& left, const CGRect& right) {
    return CGRectEqualToRect(left, right);
}
inline bool operator!= (const CGRect& left, const CGRect& right) {
    return !CGRectEqualToRect(left, right);
}

inline bool operator== (const CGSize& left, const CGSize& right) {
    return CGSizeEqualToSize(left, right);
}
inline bool operator!= (const CGSize& left, const CGSize& right) {
    return !CGSizeEqualToSize(left, right);
}

inline bool operator== (const CGPoint& left, const CGPoint& right) {
    return CGPointEqualToPoint(left, right);
}
inline bool operator!= (const CGPoint& left, const CGPoint& right) {
    return !CGPointEqualToPoint(left, right);
}


inline bool operator == (const UIEdgeInsets& left, const UIEdgeInsets& right) {
    return UIEdgeInsetsEqualToEdgeInsets(left, right);
}
inline bool operator != (const UIEdgeInsets& left, const UIEdgeInsets& right) {
    return !UIEdgeInsetsEqualToEdgeInsets(left, right);
}


class IndexPath : std::array<NSUInteger, 2> {
private:
    using Super = std::array<NSUInteger, 2>;
public:
    IndexPath(NSIndexPath *indexPath) : Super({static_cast<NSUInteger>([indexPath section]), static_cast<NSUInteger>([indexPath item])}) {}
    IndexPath(NSUInteger section, NSUInteger item) : Super({section, item}) {}
    NSUInteger section() const { return at(0); }
    void setSection(NSUInteger section) { at(0) = section; }
    NSUInteger item() const { return at(1); }
    void setItem(NSUInteger item) { at(1) = item; }
    operator NSIndexPath *() const {
        return [NSIndexPath indexPathForItem:at(1) inSection:at(0)];
    }
};

inline bool operator> (const IndexPath& left, const IndexPath& right) {
    if (left.section() > right.section()) return true;
    else if (left.section() == right.section()) return left.item() > right.item();
    else return false;
}

inline bool operator>= (const IndexPath& left, const IndexPath& right) {
    if (left.section() > right.section()) return true;
    else if (left.section() == right.section()) return left.item() >= right.item();
    else return false;
}

inline bool operator< (const IndexPath& left, const IndexPath& right) {
    return !operator>=(left, right);
}

inline bool operator<= (const IndexPath& left, const IndexPath& right) {
    return !operator>(left, right);
}

inline bool operator== (const IndexPath& left, const IndexPath& right) {
    return left.section() == right.section() && left.item() == right.item();
}

inline bool operator!= (const IndexPath& left, const IndexPath& right) {
    return !operator==(left, right);
}

template<typename T>
struct ObjcRef {
    __strong T *value;
    ObjcRef(T *val) : value(val) {}
    T *operator*() const noexcept { return value; }
    operator T*() const noexcept { return value; }
};

}

namespace std {

template<typename T>
struct hash<NE::ObjcRef<T>> {
    size_t operator()(const NE::ObjcRef<T>& obj) const noexcept {
        return [*obj hash];
    }
};

template<typename T>
bool operator==(const NE::ObjcRef<T>& left, const NE::ObjcRef<T>& right) noexcept {
    return [*left isEqual:*right];
}

template<typename T>
bool operator!=(const NE::ObjcRef<T>& left, const NE::ObjcRef<T>& right) noexcept {
    return ![*left isEqual:*right];
}

}

#endif /* NECollectionViewLayoutHelpers_h */
