#ifndef _WG_LOGGING_H
#define _WG_LOGGING_H

// Uncomment the following define to enable latency measurements
// for the sending process.
// This will generate a unique identifier for each packet to be
// sent to another peer, log timestamps and (!) insert this
// identifier into the message_data struct, so that a receiving
// peer can extract the identifier and continue logging the same
// packet.
//#define _WG_LOGGING_SEND

// Uncomment the following define to enable latency measurements
// for the receiving process.
// This will extract an identifier from the message_data struct,
// assuming (!) it was inserted there from the sending peer, and
// log timestamps using this identifier.
//#define _WG_LOGGING_RECEIVE

#define LOG_BUFSIZE	500
#define LOG_FILENAME	"/tmp/wireguard_debug.log"

#define LOG_STEP_SEND_XMIT				1
#define LOG_STEP_SEND_ENQUEUED				2
#define LOG_STEP_SEND_CONSUMED				3
#define LOG_STEP_SEND_BEFORE_ENCRYPTION			4
#define LOG_STEP_SEND_ENCRYPTED				5
#define LOG_STEP_SEND_TX_CONSUMED			6
#define LOG_STEP_SEND_SENT				7

#define LOG_STEP_RECEIVE_SOCKET				10
#define LOG_STEP_RECEIVE_ENQUEUED			11
#define LOG_STEP_RECEIVE_CONSUMED			12
#define LOG_STEP_RECEIVE_BEFORE_DECRYPTION		13
#define LOG_STEP_RECEIVE_DECRYPTED			14
#define LOG_STEP_RECEIVE_RX_CONSUMED			15
#define LOG_STEP_RECEIVE_RECEIVED			16

extern __le32 WG_MESSAGE_ID_COUNTER;

extern spinlock_t wg_log_spinlock;

void wg_log_packet(__le64 message_id, unsigned short log_step);

#endif /* _WG_LOGGING_H */
