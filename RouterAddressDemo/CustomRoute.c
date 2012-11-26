#import "CustomRoute.h"
@implementation CustomRoute

+ (NSMutableArray*) getRoutes
{
    NSMutableArray* routeArray = [NSMutableArray array];
    CustomRoute* route = nil;
    
    size_t needed;
    int mib[6];
    char *buf, *next, *lim;
    register struct rt_msghdr2 *rtm;
    
    mib[0] = CTL_NET;
    mib[1] = PF_ROUTE;
    mib[2] = 0;
    mib[3] = 0;
    mib[4] = NET_RT_DUMP2;
    mib[5] = 0;
    
    if (sysctl(mib, 6, NULL, &needed, NULL, 0) < 0) {
        err(1, "sysctl: net.route.0.0.dump estimate");
    }
    
    if ((buf = malloc(needed)) == 0) {
        err(2, "malloc(%lu)", (unsigned long)needed);
    }
    if (sysctl(mib, 6, buf, &needed, NULL, 0) < 0) {
        err(1, "sysctl: net.route.0.0.dump");
    }
    
    lim  = buf + needed;
    
    for (next = buf; next < lim; next += rtm->rtm_msglen) {
        rtm = (struct rt_msghdr2 *)next;
        route = [self getRoute:rtm];
        if(route != nil)
        {
            [routeArray addObject:route];
        }
    }
    free(buf);
    printf("Total routes: %u\n",    [routeArray count]);
    return routeArray;
}


+ (CustomRoute*) getRoute:(struct rt_msghdr2 *)rtm
{
    //sockaddrs are after the message header
    struct sockaddr* dst_sa = (struct sockaddr *)(rtm + 1);
    
    CustomRoute* route = nil;
    
    if(rtm->rtm_addrs & RTA_DST)
    {
        switch(dst_sa->sa_family)
        {
            case AF_INET:
                if(dst_sa->sa_family == AF_INET && !((rtm->rtm_flags & RTF_WASCLONED) && (rtm->rtm_parentflags & RTF_PRCLONING)))
                {
                    route = [[CustomRoute alloc] initWithRtm:rtm];
                }
                break;
                
        }
    }
    
    return route;
}

-(void) setAddr:(struct sockaddr*)sa index:(int)rtax_index
{
    if(rtax_index >= 0 && rtax_index < RTAX_MAX)
    {
        memcpy(&(m_addrs[rtax_index]), sa, sizeof(struct sockaddr));
    }
    
}

-(NSString*) getDestination
{
    return [self getAddrStringByIndex:RTAX_DST];
}

-(NSString*) getNetmask
{
    return [self getAddrStringByIndex:RTAX_NETMASK];
}

-(NSString*) getGateway
{
    return [self getAddrStringByIndex:RTAX_GATEWAY];
}

-(NSString*) getDetails
{
    NSMutableString* result = [[NSMutableString alloc] init];
    [result appendFormat: [NSString stringWithFormat: @"message type: 0x%06x\n", m_rtm.rtm_type]];
    [result appendFormat: [NSString stringWithFormat: @"flags: 0x%06x\n", m_rtm.rtm_flags]];
    [result appendFormat: [NSString stringWithFormat: @"addrs: 0x%06x\n", m_rtm.rtm_addrs]];
    
    
    return result;
}


-initWithRtm: (struct rt_msghdr2*) rtm
{
    int i;
    struct sockaddr* sa = (struct sockaddr*)(rtm + 1);
    
    
    //copy over the route message
    memcpy(&(m_rtm), rtm, sizeof(struct rt_msghdr2));
    for(i = 0; i < RTAX_MAX; i++)
    {
        [self setAddr:&(sa[i]) index:i];
    }
    return self;
}

- init
{
    memset(m_addrs, 0, sizeof(m_addrs));
    return self;
}

@end


@implementation CustomRoute (Private)

-(NSString*) getAddrStringByIndex: (int)rtax_index
{
    NSString * routeString = nil;
    struct sockaddr* sa = &(m_addrs[rtax_index]);
    int flagVal = 1 << rtax_index;
    
    if(!(m_rtm.rtm_addrs & flagVal))
    {
        return @"none";
    }
    
    
    if(rtax_index >= 0 && rtax_index < RTAX_MAX)
    {
        switch(sa->sa_family)
        {
            case AF_INET:
            {
                struct sockaddr_in* si = (struct sockaddr_in *)sa;
                if(si->sin_addr.s_addr == INADDR_ANY)
                    routeString = @"default";
                else
                    routeString = [NSString stringWithCString:(char *)inet_ntoa(si->sin_addr) encoding:NSASCIIStringEncoding];
            }
                break;
                
            case AF_LINK:
            {
                struct sockaddr_dl* sdl = (struct sockaddr_dl*)sa;
                if(sdl->sdl_nlen + sdl->sdl_alen + sdl->sdl_slen == 0)
                {
                    routeString = [NSString stringWithFormat: @"link #%d", sdl->sdl_index];
                }
                else
                    routeString = [NSString stringWithCString:link_ntoa(sdl) encoding:NSASCIIStringEncoding];
            }
                break;
                
            default:
            {
                char a[3 * sa->sa_len];
                char *cp;
                char *sep = "";
                int i;
                
                if(sa->sa_len == 0)
                {
                    routeString = @"empty";
                }
                else
                {
                    a[0] = (char)NULL;
                    for(i = 0, cp = a; i < sa->sa_len; i++)
                    {
                        cp += sprintf(cp, "%s%02x", sep, (unsigned char)sa->sa_data[i]);
                        sep = ":";
                    }
                    routeString = [NSString stringWithCString:a encoding:NSASCIIStringEncoding];
                }
            }
        }
    }
    return routeString;
}

@end