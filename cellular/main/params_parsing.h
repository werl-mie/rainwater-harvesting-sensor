// Define parameters for parsing data stream
#define LEN_RXBUF 32

#define LEN_ADDR_GW 4
#define OFFSET_ADDR_GW 7

#define LEN_ADDR_NODE  4
#define OFFSET_ADDR_NODE  14

#define LEN_VAL_POT 4
#define OFFSET_VAL_POT (OFFSET_ADDR_NODE + LEN_ADDR_NODE)

#define LEN_VAL_LVL1 2
#define OFFSET_VAL_LVL1 (OFFSET_VAL_POT + LEN_VAL_POT)

#define LEN_VAL_LVL2  2
#define OFFSET_VAL_LVL2 (OFFSET_VAL_LVL1 + LEN_VAL_LVL1)