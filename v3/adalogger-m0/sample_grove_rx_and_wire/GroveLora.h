
typedef struct struct_datum {
  uint32_t ts;
  uint16_t site_id;
  uint8_t node_id;
  uint8_t sensor_type;
  uint16_t data;
} SensorDatum_t;


class GroveLora
{
  public:
    GroveLora();
    int init(void* struct_packet);
    int at_send_check_response(char *p_ack, int timeout_ms, char*p_cmd, ...);
    int recv_prase(SensorDatum_t* p_datum);
    int node_recv(SensorDatum_t* p_datum);
    int node_send(SensorDatum_t* p_datum);
};

