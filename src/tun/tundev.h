#ifndef _TROJAN_TUNDEV_HPP
#define _TROJAN_TUNDEV_HPP

#include <list>
#include <memory>

#include <lwip/init.h>
#include <lwip/ip_addr.h>
#include <lwip/priv/tcp_priv.h>
#include <lwip/netif.h>
#include <lwip/tcp.h>
#include <lwip/ip4_frag.h>
#include <lwip/nd6.h>
#include <lwip/ip6_frag.h>

#include <linux/input.h>

#include <boost/asio/io_context.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>


class Service;
class lwip_tcp_client;
// this class canot support ipv6
class TUNDev{

private:
    static TUNDev* sm_tundev;
    static err_t static_netif_init_func(struct netif *netif){
        return sm_tundev->netif_init_func(netif);
    }

    static err_t static_netif_input_func(struct pbuf *p, struct netif *inp){
        return sm_tundev->netif_input_func(p, inp);
    }

    static err_t static_netif_output_func(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr){
        return sm_tundev->netif_output_func(netif, p, ipaddr);
    }

private:

    // lwip TUN netif device handler
    struct netif m_netif;
    bool m_netif_configured;

    // lwip TCP listener
    struct tcp_pcb *m_tcp_listener;

    err_t netif_init_func(struct netif *netif);
    err_t netif_input_func(struct pbuf *p, struct netif *inp);
    err_t netif_output_func(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr);

    err_t listener_accept_func(struct tcp_pcb *newpcb, err_t err);

private:

    std::list<std::shared_ptr<lwip_tcp_client>> m_tcp_clients;
    Service* m_service;
    int m_tun_fd;
    const bool m_is_outsize_tun_fd;
    int m_mtu;

    bool m_quitting;
    uint8_t* m_device_write_buf;

    boost::asio::streambuf m_sd_read_buffer;
    boost::asio::posix::stream_descriptor m_boost_sd;

    void async_read();

public : 
    TUNDev(Service* _service, const std::string& _tun_name, 
        const std::string& _ipaddr, const std::string& _netmask, int _mtu, int _outside_tun_fd = -1);
    ~TUNDev();
    
    int get_tun_fd(){ return m_tun_fd;}

    
};
#endif //_TROJAN_TUNDEV_HPP