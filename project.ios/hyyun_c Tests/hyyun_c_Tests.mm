//
//  hyyun_c_Tests.m
//  hyyun_c Tests
//
//  Created by brainpoint on 14-9-4.
//
//

#import <XCTest/XCTest.h>
#import "../include/hyyun_c.h"
using namespace hyyun;

#import "include/hyc/_platform/iOS.h"

const char*     ip      = "192.168.2.101";
const PORT      port    = 20000;
ProtocolSupport ps      = ProtocolSupport_IPv4;
#import "../test/hyyun_cTest_tcp.hpp"


@interface hyyun_c_Tests : XCTestCase

@end

@implementation hyyun_c_Tests

- (void)setUp
{
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testExample
{
    main();
    XCTFail(@"No implementation for \"%s\"", __PRETTY_FUNCTION__);
}

@end
