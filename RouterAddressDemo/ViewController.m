//
//  ViewController.m
//  RouterAddressDemo
//
//  Created by l on 12-11-2.
//  Copyright (c) 2012年 l. All rights reserved.
//

#import "ViewController.h"

// for "AF_INET"
#include <sys/socket.h>
//for ifaddrs
#import <netinet/in.h>
#import <netinet6/in6.h>
#import <arpa/inet.h>
#import <ifaddrs.h>
#include <netdb.h>
#import <SystemConfiguration/SCNetworkReachability.h>

#import "getgateway.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad
{
    int startTime =[[NSDate date]timeIntervalSince1970];
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    NSString* routerIP= [self routerIp];
    NSLog(@"local device ip address----%@",routerIP);
    
    
//    in_addr_t i =inet_addr("192.168.1.106");
//    in_addr_t i =inet_addr("10.27.0.4");
    in_addr_t i =inet_addr([routerIP cStringUsingEncoding:NSUTF8StringEncoding]);
    in_addr_t* x =&i;
    int r= getdefaultgateway(x);
//    NSLog(@"r--%d",r);
    int endTime =[[NSDate date]timeIntervalSince1970];
    NSLog(@"--starttime:%d,endtime:%d",startTime,endTime);
    NSLog(@" time cost getting above info is--%ds",endTime-startTime);
    
    
}
//*/
- (NSString *) routerIp {
    
    NSString *address = @"error";
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0)
    {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        //*/
        while(temp_addr != NULL)
        /*/
        int i=255;
        while((i--)>0)
         //*/
        {
            if(temp_addr->ifa_addr->sa_family == AF_INET)
            {
                // Check if interface is en0 which is the wifi connection on the iPhone
                if([[NSString stringWithUTF8String:temp_addr->ifa_name] isEqualToString:@"en0"])
                {
                    // Get NSString from C String //ifa_addr
                    //ifa->ifa_dstaddr is the broadcast address, which explains the "255's"
//                    address = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_dstaddr)->sin_addr)];
                    
                     address = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr)];
                    
                    //routerIP----192.168.1.255 广播地址
                    NSLog(@"broadcast address--%@",[NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_dstaddr)->sin_addr)]);
                    //--192.168.1.106 本机地址
                    NSLog(@"local device ip--%@",[NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr)]);
                    //--255.255.255.0 子网掩码地址
                    NSLog(@"netmask--%@",[NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_netmask)->sin_addr)]);
                    //--en0 端口地址
                    NSLog(@"interface--%@",[NSString stringWithUTF8String:temp_addr->ifa_name]);
                    
                }
            }
            
            temp_addr = temp_addr->ifa_next;
        }
    }
    
    // Free memory
    freeifaddrs(interfaces);
    
    return address;
}
//*/
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
