//
//  NECollectionViewFlowLayoutInvalidationContext.m
//  CHTCollectionViewWaterfallLayout
//
//  Created by Daniel on 2019/12/3.
//

#import "NECollectionViewFlowLayoutInvalidationContext.h"

@implementation NECollectionViewFlowLayoutInvalidationContext {
    NSMutableDictionary<NSNumber *, NSValue *> *_invalidatedSectionScrollOffsets;
}

//- (instancetype)init
//{
//    self = [super init];
//    if (self) {
//        self.invalidateFlowLayoutDelegateMetrics = NO;
//        self.invalidateFlowLayoutAttributes = NO;
//    }
//    return self;
//}

- (void)invalidateScrollOffset:(CGPoint)offset forSectionAtIndex:(NSInteger)index {
    if (_invalidatedSectionScrollOffsets == nil) {
        _invalidatedSectionScrollOffsets = [NSMutableDictionary new];
    }
    _invalidatedSectionScrollOffsets[@(index)] = @(offset);
}

- (NSDictionary<NSNumber *, NSValue *> *)invalidatedSectionScrollOffsets {
    return _invalidatedSectionScrollOffsets;
}

@end
