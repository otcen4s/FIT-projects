/***
 *** Autor: Matej Otčenáš
 *** Datum: 15.4.2020
 *** Nazov suboru: ipk-sniffer.cpp
 *** Pouzite zdroje:
 *                  https://www.tcpdump.org/pcap.html
 *                  https://tools.ietf.org/html/rfc793#page-15
 *                  https://tools.ietf.org/html/rfc768
 *                  https://tools.ietf.org/html/rfc791
 *                  https://github.com/hokiespurs/velodyne-copter/wiki/PCAP-format
 *                  https://www.devdungeon.com/content/using-libpcap-c
 *                  https://elf11.github.io/2017/01/22/libpcap-in-C.html
 *                  man 3 pcap
 ***/

#include "ipk-sniffer.h"

/**
 * Namespace pre globalne premenne
 */
namespace globals{
    std::string interface = "print all";  // -i
    long long int port = -1;  // -p
    bool port_changed = false;
    int packet_count = 1;  // -n
    bool packet_count_changed = false;
    auto tcp = false;
    auto udp = false;
    auto both = true;
}

/**
 * Funkcia pre vypis help option
 */
void Sniffer::print_help() {
    std::cout <<
              "-i eth0:             Rozhranie, na ktorom sa bude pocuvat. Ak nebude parameter uvedeny vypise zoznam aktivnych.\n"
              "-p [23]:             Filtorvanie packetov na danom rozhrani podla portu. Ak nebude parameter uvedeny, uvazuju sa vsetky porty.\n"
              "--tcp[-t]:           Zobrazi iba tcp packety.\n"  // ak nie je specifikovane udp
              "--udp[-u]:           Zobrazi iba udp packety.\n"  // ani tcp, zobrazuje oba zaroven
              "-n [10]:             Urcuje pocet packetov, ktore sa maju zobrazit, ak nie je uvedene, zobrazi sa iba 1 packet.\n"
              "--help[-h]:          Zobrazi napovedu k programu.\n";
    exit(1);
}

/**
 * Funkcia spracuje zadane vstupne argumenty programu
 */
void Sniffer::process_args(int argc, char** argv) {
    const char* const short_opts = "i::p::n::tuh";
    const option long_opts[] = {
            {"tcp", no_argument, nullptr, 't'},
            {"udp", no_argument, nullptr, 'u'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, no_argument, nullptr, 0}
    };

    while (true){
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (opt == -1) break;  // koniec citania argumentov

        std::string arg;

        switch (opt){
            case 'i':
                if(!optarg && NULL != argv[optind] && '-' != argv[optind][0] ) {
                    globals::interface = argv[optind];
                }
                break;

            case 'p':
                if(!optarg && NULL != argv[optind] && '-' != argv[optind][0] ) {
                    globals::port = std::stoi(argv[optind]);
                    globals::port_changed = true;
                }
                break;

            case 'n':
                if(!optarg && NULL != argv[optind] && '-' != argv[optind][0] ) {
                    globals::packet_count = std::stoi(argv[optind]);
                    globals::packet_count_changed = true;
                }
                break;

            case 't':
                globals::tcp = true;
                if(globals::udp) globals::both = true;
                else globals::both = false;
                break;

            case 'u':
                globals::udp = true;
                if(globals::tcp) globals::both = true;
                else globals::both = false;
                break;

            case 'h':
                print_help();
                break;

            case '?':
                if(optopt == 'i' || optopt == 'n' || optopt == 'p')
                    break;
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
            default:
                exit(1);
                break;

        }
    }
}

/**
 * Funkcia vyhlada vsetky dostupne rozhrania
 */
void Sniffer::lookup_all_interfaces(pcap_if_t * all_interfaces) {
    int i=0;
    char errbuf[PCAP_ERRBUF_SIZE];

    if (pcap_findalldevs(&all_interfaces, errbuf) == -1){
        fprintf(stderr,"Error while finding all devices: %s\n", errbuf);
        exit(1);
    }

    // Vypise zoznam aktivnych zariadeni(popripade aj blizsi popis)
    for(pcap_if_t* dev = all_interfaces; dev != NULL; dev = dev->next){
        printf("%d. %s", ++i, dev->name);
        if (dev->description)
            printf(" (%s)\n", dev->description);
        else
            printf(" (No description available)\n");
    }
    pcap_freealldevs(all_interfaces);
}

/**
 * Funkcia nastavuje potrebne parametre pre zadany interface pouzitim knihovnych funkcii pcap.h
 * Nastavenie interface je napisane pomocou dokumentu, zdroj: https://www.tcpdump.org/pcap.html
 */
pcap_t * Sniffer::set_interface(char* device, bool filter_all) {
    // premenne pre pracu s <pcap.h> kniznicou
    pcap_t *handle;		            // error handler (kontrola pre NULL)
    char errbuf[PCAP_ERRBUF_SIZE];  // error buffer na zachytavanie chybovej spravy
    struct bpf_program fp;		    // struktura pre filter
    char filter_exp[20] = "port ";	// vyraz filtru
    bpf_u_int32 mask;		        // maska pre snifing zariadenie
    bpf_u_int32 net;		        // ip nasho sniffing zariadenia

    if (pcap_lookupnet(device, &net, &mask, errbuf) < 0) {
        fprintf(stderr, "Can't get netmask for device %s!\n", device);
        exit(EXIT_FAILURE);
    }

    handle = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);  // otovri rozhranie a urci pocet bytov na citanie a nastavi ostatne priznaky
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", device, errbuf);
        exit(EXIT_FAILURE);
    }

    // nastavi sa filter pre zadany port
    if (!filter_all){
        std::string port_s = std::to_string(globals::port);  // konverzia zo std::string na char[] a este z int do std::string
        char port [port_s.size() + 1];
        std::copy(port_s.begin(), port_s.end(), port);
        port[port_s.size()] = '\0';
        strcat(filter_exp, port);

        if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
            fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
            exit(EXIT_FAILURE);
        }

        if (pcap_setfilter(handle, &fp) == -1) {
            fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
            exit(EXIT_FAILURE);
        }
        pcap_freecode(&fp);
    }

    return handle;
}

/**
 * Funkcia ziska FQDN(fully qualified domain name) IP adresy ak je dostupne
 */
std::string Sniffer::get_hostname(char ip[INET6_ADDRSTRLEN], sa_family_t family,  u_int port){
    char hostname[NI_MAXHOST];
    struct sockaddr_in sock_addr;

    sock_addr.sin_family = family;
    inet_pton(family, ip, &sock_addr.sin_addr);
    int ret_val = getnameinfo((struct sockaddr *)&sock_addr, sizeof(struct sockaddr), hostname, NI_MAXHOST,  nullptr, 0, NI_IDN | NI_NAMEREQD);

    if (ret_val == EAI_NONAME || ret_val != 0) return "";
    else return hostname;
}

/**
 * Toto je callback funkcia, ktora sa pouzije ako parameter vo funckii pcap_loop(), aplikuje sa pre kazdy packet
 */
void Sniffer::Callback(u_char *user_data, const struct pcap_pkthdr* packet_header, const u_char* packet) {
    if(globals::packet_count < 0) return;  // pre vypis presneho poctu packetov

    char source_IP[INET6_ADDRSTRLEN];  // vytvorime buffer s velkostou pre ipv6 pre istotu ak by bola adresa vo formate ipv6
    char dest_IP[INET6_ADDRSTRLEN];
    u_int source_port, dest_port;
    const struct ip* ip_header;
    int ip_header_length;
    const struct tcphdr* tcp_header;
    const struct udphdr* udp_header;
    int ethernet_header_length = ETHERNET_HEADER_LEN; // fixna velkost hlavicky dana protokolom
    const struct ether_header* ethernet_header = (struct ether_header*)packet;
    sa_family_t family;


    // pre rozhodovanie o type IP adresy a samotne ziskanie portov a IP adries
    // som vyuzil zdroj: https://elf11.github.io/2017/01/22/libpcap-in-C.html
    if(ntohs(ethernet_header->ether_type) == ETHERTYPE_IP) {  // IPV4
        ip_header = (struct ip *) (packet + sizeof(struct ether_header));
        inet_ntop(AF_INET, &(ip_header->ip_src), source_IP, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_header->ip_dst), dest_IP, INET_ADDRSTRLEN);
        family = AF_INET;
        ip_header_length = IPV4_HEADER_LEN;
        //ip_header_length = ((*(packet + ethernet_header_length)) & 0x0F) * 4;   // posun ukazatela v pamati na zaciatok novej vrstvy IP layer
                                                                                  // logicka operacia AND, kde 0x0F = 0000 1111
                                                                                  // druha cast z 8 bitov *ip_header obsahuje velkost IP hlavicky
                                                                                  // IHL(IP header length) vynasobime 4 a ziskame velkost v bytoch
    }
    else if(ntohs(ethernet_header->ether_type) == ETHERTYPE_IPV6){  // IPV6
        ip_header = (struct ip *) (packet + sizeof(struct ether_header));
        inet_ntop(AF_INET6, &(ip_header->ip_src), source_IP, INET6_ADDRSTRLEN);
        inet_ntop(AF_INET6, &(ip_header->ip_dst), dest_IP, INET6_ADDRSTRLEN);
        family = AF_INET6;
        ip_header_length = IPV6_HEADER_LEN;
    }
    else return;

    // buffre na ukladanie informacii pre vypis na prvy riadok
    std::string info_src = "";
    std::string info_dest = "";


    // budeme testovat len pre dva pripady podla zadania TCP/UDP
    if (ip_header->ip_p == IPPROTO_TCP) {
        globals::packet_count--;
        if(globals::both || globals::tcp){  // mozme spracovat TCP
            tcp_header = (struct tcphdr*)(packet + sizeof(struct ether_header) + sizeof(struct ip));
            source_port = ntohs(tcp_header->source);
            dest_port = ntohs(tcp_header->dest);


            info_src = get_hostname(source_IP, family, source_port);
            if(info_src == "")  info_src += static_cast<std::string>(source_IP);
            info_src += ": ";

            info_dest = get_hostname(dest_IP, family, dest_port);
            if(info_dest == "")  info_dest += static_cast<std::string>(dest_IP);
            info_dest += ": ";


            // konverzia portov a ulozenie do bufferov
            char tmp[80];
            sprintf(tmp, "%d", source_port);
            info_src += tmp;
            sprintf(tmp, "%d", dest_port);
            info_dest += tmp;

            std::string info;

            // ziasknie a ulozenie casu ziskania packetu do bufferu
            timeval cur_time;
            gettimeofday(&cur_time, NULL);
            int milli = cur_time.tv_usec;
            char buffer [80];
            strftime(buffer, 80, "%H:%M:%S", localtime(&cur_time.tv_sec));

            char current_time[93] = "";
            sprintf(current_time, "%s.%06d ", buffer, milli);
            info = current_time;
            info += info_src + " > " + info_dest;

            std::cout<<info<<std::endl<<std::endl;  // vypis informacii

            print_tcp(packet, ethernet_header_length, ip_header_length, packet_header); // spracovanie tcp packetu
        }
        return;
    }
    else if(ip_header->ip_p == IPPROTO_UDP){
        globals::packet_count--;
        if(globals::both || globals::udp){  // mozme spracovat UDP
            udp_header = (struct udphdr*)(packet + sizeof(struct ether_header) + sizeof(struct ip));
            source_port = ntohs(udp_header->source);
            dest_port = ntohs(udp_header->dest);

            info_src = get_hostname(source_IP, family, source_port);
            if(info_src == "")  info_src += static_cast<std::string>(source_IP);
            info_src += ": ";

            info_dest = get_hostname(dest_IP, family, dest_port);
            if(info_dest == "")  info_dest += static_cast<std::string>(dest_IP);
            info_dest += ": ";

            // konverzia portov a ulozenie do bufferov
            char tmp[80];
            sprintf(tmp, "%d", source_port);
            info_src += tmp;
            sprintf(tmp, "%d", dest_port);
            info_dest += tmp;

            std::string info;

            // ziasknie a ulozenie casu ziskania packetu do bufferu
            timeval cur_time;
            gettimeofday(&cur_time, NULL);
            int milli = cur_time.tv_usec;
            char buffer [80];
            strftime(buffer, 80, "%H:%M:%S", localtime(&cur_time.tv_sec));

            char current_time[93] = "";
            sprintf(current_time, "%s.%06d ", buffer, milli);
            info = current_time;
            info += info_src + " > " + info_dest;

            std::cout<<info<<std::endl<<std::endl;

            print_udp(packet, ethernet_header_length, ip_header_length, packet_header);
        }
        return;
    }
}

/**
 * Funkcia spracuje a vypise UDP packet (hlavicka, payload)
 */
void Sniffer::print_udp(const u_char *packet, int ethernet_header_length, int ip_header_length, const struct pcap_pkthdr *packet_header) {
    const u_char* udp_header;  // hlavicka udp_header
    const u_char* payload;     // vsetko za castou hlavicka (header)
    int udp_header_length;     // velkost udp hlavicky v bytoch
    int payload_length;        // velkost spravy za hlavickou v bytoch
    int total_headers_size;

    udp_header = packet + ethernet_header_length + ip_header_length;
    udp_header_length = UDP_HEADER_LEN;

    total_headers_size = ethernet_header_length + ip_header_length + udp_header_length;

    payload_length = packet_header->caplen - total_headers_size;
    payload = packet + total_headers_size; // posun ukazatela na zaciatok dat pre payload

    // vypis UDP hlavicky
    print_data(udp_header_length, udp_header);

    // vypis UDP payload
    print_data(payload_length, payload);
}

/**
 * Funkcia spracuje a vypise TCP packet (hlavicka, payload)
 */
void Sniffer::print_tcp(const u_char* packet, int ethernet_header_length, int ip_header_length, const struct pcap_pkthdr* packet_header) {
    const u_char* tcp_header;  // hlavicka tcp_header
    const u_char* payload;     // vsetko za castou hlavicka (header)
    int tcp_header_length;     // velkost tcp hlavicky v bytoch
    int payload_length;        // velkost spravy za hlavickou v bytoch

    tcp_header = packet + ethernet_header_length + ip_header_length;  // k paketu pripocitame velkost ethernet_header a ip header
                                                                      // a dostaneme ukazatel na zaciatok tcp hlavicky

    tcp_header_length = ((*(tcp_header + 12)) & 0xF0) >> 4;           // velkost hlavicky je ulozena v prvej polovici 12 bytu
                                                                      // preto pomocou botovej operacie AND mmusime pouzit bitovy
                                                                      // posun o 4 bity vpravo aby sme ziskali LSB bity
                                                                      // note: na ziskanie velkosti tcp hlavicky som sa inspiroval
                                                                      // na stranke https://www.devdungeon.com/content/using-libpcap-c

    tcp_header_length = tcp_header_length * 4;                        // rovnako ako pri IP header vynasobime * 4 pre konverziu na bajty(pri IP header to je nahradene za konstantu velkost 20)
    int total_headers_size = ethernet_header_length + ip_header_length + tcp_header_length;  // velkost vsetkych hlaviciek pouzijeme na posun k payload

    payload_length = packet_header->caplen - total_headers_size;  //  dokumentacia <pcap> pre caplen: "giving the number of bytes of the packet that are available from the capture"
                                                                                                               // velkost payload ziskame teda pomocou predoslych vypoctov
    payload = packet + total_headers_size;  // posun ukazatela na zaciatok dat pre payload

    // vypis TCP hlavicky
    print_data(tcp_header_length, tcp_header);

    // vypis TCP payload
    print_data(payload_length, payload);
}

/**
 * Funkcia vypisuje zadane data v predurcenom formate (data mozu byt v tomto pripade payload alebo hlavicka)
 */
void Sniffer::print_data(int length, const u_char *data) {
    if(length > 0){
        std::string tmp_buffer;
        const u_char *ptr = data;
        int byte_count = 0;
        int cnt = 0;
        int hexa_length = 0;
        while (byte_count++ < length) {
            if (cnt % 16 == 0) printf("0x%04x:  ", cnt);

            if(isprint(*ptr)) tmp_buffer += *ptr;
            else tmp_buffer += ".";

            char buff[3];
            sprintf(buff, "%x", *ptr);
            if(strlen(buff) == 1) printf("0%x ",*ptr);
            else printf("%x ",*ptr);

            if(byte_count % 8 == 0 && byte_count != 0) {
                std::cout<<" ";
                tmp_buffer += " ";
            }

            if((byte_count % 16 == 0 && byte_count != 0) || byte_count == length) {
                if(byte_count == length){
                    int spaces = 48 - hexa_length;
                    if (byte_count % 16 == 0) spaces--;
                    if (hexa_length < 21) spaces++;
                    for (int i = 0; i < spaces - 1; ++i) {
                        std::cout<<" ";
                    }
                    std::cout<<tmp_buffer<<std::endl;
                }
                else{
                    std::cout<<" "<<tmp_buffer<<std::endl;
                    tmp_buffer.clear();
                    ptr++;
                    cnt++;
                    hexa_length = 0;
                    continue;
                }
            }
            ptr++;
            cnt++;
            hexa_length += 3;
        }
        printf("\n\n");
    }
}

/**
 * Funkcia zavola funckie kniznice pcap.h
 */
void Sniffer::capture_packet(pcap_t *handle) {
    pcap_loop(handle, globals::packet_count, this->Callback, NULL);
    pcap_close(handle);
}


/***********************************************/
/******************MAIN*************************/
/***********************************************/
int main(int argc, char** argv){
    Sniffer sniffer;                    // objekt sniffer
    sniffer.process_args(argc, argv);  // argument parsing
    bool filter_all;

    // kontrola zadaneho poctu packetov
    if(globals::packet_count_changed){
        if(globals::packet_count < 1){
            std::cerr<<"Packet count can't be less than 1"<<std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // kontrola rozsahu cisla portu
    if(globals::port_changed){
        if(globals::port < 0 || globals::port > 65535) {
            std::cerr<<"Size of port is not uint16_t"<<std::endl;
            exit(EXIT_FAILURE);
        }
        filter_all = false;
    }
    else filter_all = true;   // sniff na vsetkych portoch


    // vypise vsetky dostupne interfaces
    if(globals::interface == "print all"){
        pcap_if_t *all_interfaces = nullptr;
        sniffer.lookup_all_interfaces(all_interfaces);
    }
    // nastavi zadany platny interface
    else{
        pcap_t *handle;

        char interface [globals::interface.size() + 1];
        std::copy(globals::interface.begin(), globals::interface.end(), interface);
        interface[globals::interface.size()] = '\0';

        handle = sniffer.set_interface(interface, filter_all);

        sniffer.capture_packet(handle);
    }

    exit(EXIT_SUCCESS);
}
