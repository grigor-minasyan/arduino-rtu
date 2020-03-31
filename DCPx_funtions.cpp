/*
typedef struct _DCP_OP_NTRY {
  byte text[5];
  byte code;
  byte lne;;
} DCP_OP_NTRY;
typedef struct _DCP_OP_NTRY KS_FAR * PDCP_OP_NTRY;

DCP_OP_NTRY KS_FAR DCP_op_table[] = {
  {"UPDR", DCP_UPDRop, 2},
  {"FUDR", DCP_FUDRop, 2},
  {"GTIM", DCP_GTIMop, 2},
  {{0}, {0}, {0}},
};
PDCP_OP_NTRY DCP_op_lookup(byte op) {
  byte i = 0;
  for( ; DCP_op_table[i].code != 0 && DCP_op_table[i].code != op; i++);
  return (&DCP_op_table[i]);
}

*/
