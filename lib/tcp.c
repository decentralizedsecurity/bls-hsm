#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>
#include <modem/nrf_modem_lib.h>
#include <modem/lte_lc.h>
#include <zephyr/net/tls_credentials.h>
#include <modem/modem_key_mgmt.h>

LOG_MODULE_REGISTER(tcp_module, LOG_LEVEL_INF);
#define TCP_PORT 9090
#define HOSTNAME "blshsm.ddns.net"

void modem_configure(void)
{
printk("Waiting for network.. ");
	int err = lte_lc_init_and_connect();
	if (err) {
		printk("Failed to connect to the LTE network, err %d\n", err);
		return;
	}
	printk("OK\n");
}

int cred_delete(int slot, int type){
	int err = modem_key_mgmt_delete(slot, type);
	if (err) {
		printk("Failed to delete existing certificate, err %d\n", err);
		return err;
	}
	return 0;
}

int cred_exists(int slot, int type){
	bool exists;
	int err = modem_key_mgmt_exists(slot, type, &exists);
	if (err) {
		printk("Failed to check for certificates err %d\n", err);
		return err;
	}
	if(exists){
		return 1;
	}
	return 0;
}

int recv_tcp(int fd, char* buf, int bufsize){
	ssize_t len = recv(fd, buf, bufsize, 0);

	if (len < 0) {
		printk("recv() failed\n");
		return len;
	}

	printf("Received %ld bytes\n", (long)len);
	buf[len] = '\0';
	printk("%s", buf);
	return len;
}

int send_tcp(int fd, char* buf, ssize_t len){
	ssize_t sentlen = 0;
	for(;;){
		ssize_t ret = send(fd, buf + sentlen, len - sentlen, 0);

		if (ret < 0) {
			printk("send() failed\n");
			return ret;
		}

		printf("Sent %ld bytes\n", (long)ret);

		sentlen += ret;

		if(len - sentlen == 0){
			break;
		}
	}
	return 0;
}

/* Setup TLS options on a given socket */
int tls_setup(int fd, int slot)
{
	int err;
	int verify;

	/* Security tag that we have provisioned the certificate with */
	const sec_tag_t tls_sec_tag[] = {
		slot,
	};

#if defined(CONFIG_SAMPLE_TFM_MBEDTLS)
	err = tls_credential_add(tls_sec_tag[0], TLS_CREDENTIAL_CA_CERTIFICATE, cert, sizeof(cert));
	if (err) {
		return err;
	}
#endif

	/* Set up TLS peer verification */
	enum {
		NONE = 0,
		OPTIONAL = 1,
		REQUIRED = 2,
	};

	verify = REQUIRED;

	err = setsockopt(fd, SOL_TLS, TLS_PEER_VERIFY, &verify, sizeof(verify));
	if (err) {
		printk("Failed to setup peer verification, err %d\n", errno);
		return err;
	}

	/* Associate the socket with the security tag
	 * we have provisioned the certificate with.
	 */
	err = setsockopt(fd, SOL_TLS, TLS_SEC_TAG_LIST, tls_sec_tag,
			 sizeof(tls_sec_tag));
	if (err) {
		printk("Failed to setup TLS sec tag, err %d\n", errno);
		return err;
	}

	err = setsockopt(fd, SOL_TLS, TLS_HOSTNAME, HOSTNAME, sizeof(HOSTNAME) - 1);
	if (err) {
		printk("Failed to setup TLS hostname, err %d\n", errno);
		return err;
	}
	return 0;
}