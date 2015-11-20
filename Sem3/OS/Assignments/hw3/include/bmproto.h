#ifndef __BMPROTO_H
#define __BMPROTO_H

#define BMP_SETUP 0
#define BMP_REPLY 1
#define BMP_OPERATION 2
#define BMP_DESTROY 4

#define BMP_FAILURE 0
#define BMP_SUCCESS 1

#define BMP_PRODUCE 0
#define BMP_CONSUME 1

typedef struct {
	int type;
	union {
		struct { // SETUP
			int operation_type; // Producer or Consumer
		} setup_msg;
		struct { // REPLY
			int status;         // SUCCESS or FAILURE
			int selection;      // Buffer
			int value;          // Value
		} reply_msg;
		struct { // OPERATION
			int operation_type; 
			int selection;
		} operation_msg;
	} message;
} bmproto_t;

/* Creation Functions */
extern bmproto_t bmp_setup(int optype);
extern bmproto_t bmp_reply(int status, int selection, int value);
extern bmproto_t bmp_operation(int optype, int selection); 
extern bmproto_t bmp_produce(int selection);
extern bmproto_t bmp_consume(int selection);
extern bmproto_t bmp_destroy();

/* Value Extraction Functions */
extern int bmp_optype(bmproto_t proto);
extern int bmp_selection(bmproto_t proto);
extern int bmp_value(bmproto_t proto);
extern int bmp_status(bmproto_t proto);

extern void bmp_print(const bmproto_t msg);


#endif
