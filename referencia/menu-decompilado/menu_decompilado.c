/* Pseudocódigo exportado pelo Ghidra a partir de binary/picostation-menu.bin.
 * MIPS R3000 little-endian. Mapeamento PS-X EXE a partir de 0x80010000.
 * Os nomes, os tipos e o fluxo de controle são inferidos pelo decompilador.
 */

/* 0x80010000 _ftext */
/* WARNING: Removing unreachable block (ram,0x80010168) */
/* WARNING: Removing unreachable block (ram,0x80010178) */
/* WARNING: Removing unreachable block (ram,0x800100f0) */
/* WARNING: Removing unreachable block (ram,0x80010114) */
/* WARNING: Removing unreachable block (ram,0x80010120) */
/* WARNING: Removing unreachable block (ram,0x800100e8) */
/* WARNING: Removing unreachable block (ram,0x8001012c) */
/* WARNING: Removing unreachable block (ram,0x80010108) */
/* WARNING: Removing unreachable block (ram,0x8001018c) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void _ftext(uint param_1)

{
  _DAT_bf801814 = (param_1 & 1) << 3 | 0x8000001;
  return;
}

/* 0x8001034c FUN_8001034c */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8001034c(void)

{
  do {
  } while ((_DAT_bf801814 & 0x4000000) == 0);
  return;
}

/* 0x80010398 FUN_80010398 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80010398(void)

{
  do {
  } while ((_DAT_bf8010a8 & 0x1000000) != 0);
  return;
}

/* 0x800103e4 FUN_800103e4 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_800103e4(uint param_1)

{
  FUN_80010398();
  if ((param_1 & 3) != 0) {
    FUN_80019498("/home/megavolt/SRC/picostation-menu-line/src/gpu.c",0x3d,"!((uint32_t) data % 4)")
    ;
  }
  _DAT_bf8010a0 = param_1;
  _DAT_bf8010a8 = 0x1000401;
  return;
}

/* 0x80010474 FUN_80010474 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80010474(uint param_1,undefined4 param_2,undefined4 param_3,uint param_4,int param_5)

{
  uint uVar1;
  uint local_28;
  uint local_24;
  
  FUN_80010398();
  if ((param_1 & 3) != 0) {
    FUN_80019498("/home/megavolt/SRC/picostation-menu-line/src/gpu.c",0x45,"!((uint32_t) data % 4)")
    ;
  }
  uVar1 = (int)(param_4 * param_5) / 2;
  if (uVar1 < 0x10) {
    local_24 = 1;
    local_28 = uVar1;
  }
  else {
    local_28 = 0x10;
    local_24 = uVar1 >> 4;
    if ((uVar1 & 0xf) != 0) {
      FUN_80019498("/home/megavolt/SRC/picostation-menu-line/src/gpu.c",0x51,
                   "!(length % DMA_MAX_CHUNK_SIZE)");
    }
  }
  FUN_8001034c();
  _DAT_bf801810 = param_4 & 0xffff | param_5 << 0x10;
  _DAT_bf8010a0 = param_1;
  _DAT_bf8010a4 = local_24 << 0x10 | local_28;
  _DAT_bf8010a8 = 0x1000201;
  return;
}

/* 0x80010668 FUN_80010668 */
uint * FUN_80010668(int param_1,int param_2)

{
  uint *puVar1;
  
  puVar1 = *(uint **)(param_1 + 0x10000);
  *(int *)(param_1 + 0x10000) = *(int *)(param_1 + 0x10000) + (param_2 + 1) * 4;
  *puVar1 = *(uint *)(param_1 + 0x10000) & 0xffffff | param_2 << 0x18;
  if (param_1 + 0x10000U <= *(uint *)(param_1 + 0x10000)) {
    FUN_80019498("/home/megavolt/SRC/picostation-menu-line/src/gpu.c",99,
                 "chain->nextPacket < &(chain->data)[CHAIN_BUFFER_SIZE]");
  }
  return puVar1 + 1;
}

/* 0x8001077c FUN_8001077c */
void FUN_8001077c(undefined1 *param_1,undefined4 param_2,uint param_3,int param_4,int param_5,
                 int param_6)

{
  uint uVar1;
  int iVar2;
  
  if ((0x100 < param_5) || (0x100 < param_6)) {
    FUN_80019498("/home/megavolt/SRC/picostation-menu-line/src/gpu.c",0x6b,
                 "(width <= 256) && (height <= 256)");
  }
  FUN_80010474(param_2,param_3,param_4,param_5,param_6);
  FUN_80010398();
  uVar1 = param_3;
  if ((int)param_3 < 0) {
    uVar1 = param_3 + 0x3f;
  }
  iVar2 = param_4;
  if (param_4 < 0) {
    iVar2 = param_4 + 0xff;
  }
  *(ushort *)(param_1 + 6) =
       (ushort)((int)uVar1 >> 6) & 0xf | (ushort)((iVar2 >> 8 & 1U) << 4) | 0x100 |
       (ushort)((iVar2 >> 8 & 2U) << 10);
  *(undefined2 *)(param_1 + 8) = 0;
  param_3 = param_3 & 0x8000003f;
  if ((int)param_3 < 0) {
    param_3 = (param_3 - 1 | 0xffffffc0) + 1;
  }
  *param_1 = (char)param_3;
  param_1[1] = (char)param_4;
  *(short *)(param_1 + 2) = (short)param_5;
  *(short *)(param_1 + 4) = (short)param_6;
  return;
}

/* 0x800109cc FUN_800109cc */
void FUN_800109cc(char *param_1,undefined4 param_2,undefined4 param_3,uint param_4,int param_5,
                 uint param_6,uint param_7,int param_8,int param_9,uint param_10)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  
  if ((0x100 < param_8) || (0x100 < param_9)) {
    FUN_80019498("/home/megavolt/SRC/picostation-menu-line/src/gpu.c",0x7e,
                 "(width <= 256) && (height <= 256)");
  }
  if (param_10 == 1) {
    iVar1 = 0x100;
    iVar2 = 2;
  }
  else {
    iVar1 = 0x10;
    iVar2 = 4;
  }
  if (((param_6 & 0xf) != 0) || (0x400 < (int)(param_6 + iVar1))) {
    FUN_80019498("/home/megavolt/SRC/picostation-menu-line/src/gpu.c",0x83,
                 "!(paletteX % 16) && ((paletteX + numColors) <= 1024)");
  }
  FUN_80010474(param_2,param_4,param_5,param_8 / iVar2,param_9);
  FUN_80010398();
  FUN_80010474(param_3,param_6,param_7,iVar1,1);
  FUN_80010398();
  uVar3 = param_4;
  if ((int)param_4 < 0) {
    uVar3 = param_4 + 0x3f;
  }
  iVar1 = param_5;
  if (param_5 < 0) {
    iVar1 = param_5 + 0xff;
  }
  *(ushort *)(param_1 + 6) =
       (ushort)((int)uVar3 >> 6) & 0xf | (ushort)((iVar1 >> 8 & 1U) << 4) |
       (ushort)((param_10 & 3) << 7) | (ushort)((iVar1 >> 8 & 2U) << 10);
  if ((int)param_6 < 0) {
    param_6 = param_6 + 0xf;
  }
  *(ushort *)(param_1 + 8) = (ushort)((int)param_6 >> 4) & 0x3f | (ushort)((param_7 & 0x3ff) << 6);
  param_4 = param_4 & 0x8000003f;
  if ((int)param_4 < 0) {
    param_4 = (param_4 - 1 | 0xffffffc0) + 1;
  }
  *param_1 = (char)param_4 * (char)iVar2;
  param_1[1] = (char)param_5;
  *(short *)(param_1 + 2) = (short)param_8;
  *(short *)(param_1 + 4) = (short)param_9;
  return;
}

/* 0x80010d64 FUN_80010d64 */
void FUN_80010d64(byte param_1,undefined2 param_2)

{
  undefined1 local_10;
  byte local_f;
  undefined1 local_e;
  undefined1 local_d;
  
  local_10 = 0x50;
  local_f = param_1 | 0xf0;
  local_e = (undefined1)((ushort)param_2 >> 8);
  local_d = (undefined1)param_2;
  FUN_80016330(0x19,&local_10,4);
  return;
}

/* 0x80010e00 FUN_80010e00 */
int FUN_80010e00(byte *param_1)

{
  uint uVar1;
  byte *local_res0;
  int local_18;
  byte local_14;
  
  local_18 = 0;
  local_res0 = param_1;
  do {
    if (*local_res0 == 0) {
      return local_18;
    }
    local_14 = *local_res0;
    if (local_14 == 0x20) {
      local_18 = local_18 + 4;
    }
    else if (local_14 < 0x21) {
      if (local_14 == 9) {
        uVar1 = local_18 + 0x1fU & 0x8000001f;
        if ((int)uVar1 < 0) {
          uVar1 = (uVar1 - 1 | 0xffffffe0) + 1;
        }
        local_18 = (local_18 + 0x1fU) - uVar1;
      }
      else if (local_14 != 10) goto LAB_80010ed8;
    }
    else {
LAB_80010ed8:
      if (0x98 < local_14) {
        local_14 = 0x7f;
      }
      local_18 = local_18 + (uint)(byte)(&DAT_8001cd12)[(local_14 - 0x21) * 4];
    }
    local_res0 = local_res0 + 1;
  } while( true );
}

/* 0x80010f84 FUN_80010f84 */
void FUN_80010f84(undefined4 param_1,byte *param_2,uint param_3,int param_4,byte *param_5,
                 int param_6)

{
  uint *puVar1;
  uint uVar2;
  undefined4 *puVar3;
  int iVar4;
  uint local_48;
  int local_44;
  byte local_40;
  
  puVar1 = (uint *)FUN_80010668(param_1,1);
  *puVar1 = *(ushort *)(param_2 + 6) & 0x9ff | 0xe1000000;
  local_48 = param_3;
  local_44 = param_4;
  do {
    if (*param_5 == 0) {
      return;
    }
    local_40 = *param_5;
    if (local_40 == 0x20) {
      local_48 = local_48 + 4;
    }
    else if (local_40 < 0x21) {
      if (local_40 == 9) {
        uVar2 = local_48 + 0x1f & 0x8000001f;
        if ((int)uVar2 < 0) {
          uVar2 = (uVar2 - 1 | 0xffffffe0) + 1;
        }
        local_48 = (local_48 + 0x1f) - uVar2;
      }
      else {
        if (local_40 != 10) goto LAB_80011120;
        local_44 = local_44 + 10;
        local_48 = param_3;
      }
    }
    else {
LAB_80011120:
      if (0x98 < local_40) {
        local_40 = 0x7f;
      }
      iVar4 = (local_40 - 0x21) * 4;
      if (param_6 < (int)((byte)(&DAT_8001cd12)[iVar4] + local_48)) {
        return;
      }
      puVar3 = (undefined4 *)FUN_80010668(param_1,4);
      *puVar3 = 0x67000000;
      puVar3[1] = local_48 & 0xffff | local_44 << 0x10;
      puVar3[2] = (uint)*param_2 + (uint)(byte)(&UNK_8001cd10)[iVar4] & 0xff |
                  ((uint)param_2[1] + (uint)(byte)(&UNK_8001cd11)[iVar4] & 0xff) << 8 |
                  (uint)*(ushort *)(param_2 + 8) << 0x10;
      puVar3[3] = (uint)(byte)(&DAT_8001cd12)[iVar4] | (uint)(byte)(&UNK_8001cd13)[iVar4] << 0x10;
      local_48 = local_48 + (byte)(&DAT_8001cd12)[iVar4];
    }
    param_5 = param_5 + 1;
  } while( true );
}

/* 0x80011420 FUN_80011420 */
void FUN_80011420(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5)

{
  FUN_80010f84(param_1,param_2,param_3,param_4,param_5,0x140);
  return;
}

/* 0x80011488 FUN_80011488 */
undefined4 FUN_80011488(ushort *param_1,int param_2)

{
  ushort uVar1;
  ushort local_18;
  
  local_18 = 0;
  while( true ) {
    if ((0x913 < local_18) || (0xfff < *param_1)) {
      return 0;
    }
    uVar1 = (ushort)*(byte *)(param_2 + (uint)local_18);
    if (uVar1 == 0) break;
    FUN_80014424(*param_1,*(undefined1 *)(param_2 + local_18 + 1),param_2 + local_18 + 2,uVar1);
    local_18 = local_18 + uVar1 + 2;
    *param_1 = *param_1 + 1;
  }
  if ((*(char *)(param_2 + local_18 + 2) == -2) && (*(char *)(param_2 + local_18 + 3) == -2)) {
    return 0;
  }
  if ((*(char *)(param_2 + local_18 + 1) != '\x01') &&
     ((*(char *)(param_2 + local_18 + 2) != '\0' || (*(char *)(param_2 + local_18 + 3) != '\0')))) {
    return 0;
  }
  return 1;
}

/* 0x800116e4 FUN_800116e4 */
short FUN_800116e4(int param_1,undefined1 param_2,short param_3)

{
  undefined1 local_res4;
  short local_res8;
  char local_10;
  short local_e [3];
  
  local_e[0] = 0;
  local_10 = '\x01';
  local_res4 = param_2;
  local_res8 = param_3;
  while (local_10 != '\0') {
    FUN_80010d64(local_res4,local_res8);
    FUN_8001657c(100,param_1,1,0x924,1,1);
    local_10 = FUN_80011488(local_e,param_1 + 0xc);
    local_res4 = 4;
    local_res8 = local_e[0];
  }
  if (local_e[0] != 0) {
    FUN_800145b8(local_e[0]);
    FUN_80014124(local_e);
  }
  return local_e[0];
}

/* 0x800117f8 FUN_800117f8 */
void FUN_800117f8(void)

{
  int iVar1;
  char local_18;
  undefined1 local_10 [8];
  
  FUN_80010d64(8,6);
  local_18 = '\0';
  do {
    if ('\x04' < local_18) {
LAB_800119fc:
      FUN_80010d64(0,0);
      return;
    }
    FUN_8001657c(0x96,&DAT_80022dd0,1,0x924,1,1);
    iVar1 = FUN_8001997c(&DAT_80022ddc,&DAT_8001cef0,4);
    if (iVar1 == 0) goto LAB_800119fc;
    iVar1 = FUN_8001997c(&DAT_80022ddc,&DAT_8001cef8,4);
    if (iVar1 == 0) {
      iVar1 = FUN_80014768(&DAT_80022ef4,"background",local_10);
      if (iVar1 != 0) {
        DAT_80022d70 = SUB41(local_10._0_4_,2);
        DAT_80022d71 = SUB41(local_10._0_4_,1);
        DAT_80022d4c = (undefined1)local_10._0_4_;
      }
      iVar1 = FUN_80014768(&DAT_80022ef4,"frame",local_10);
      if (iVar1 != 0) {
        DAT_80022d4d = SUB41(local_10._0_4_,2);
        DAT_80022d4e = SUB41(local_10._0_4_,1);
        DAT_80022d4f = (undefined1)local_10._0_4_;
      }
      iVar1 = FUN_80014768(&DAT_80022ef4,"filelist",local_10);
      if (iVar1 != 0) {
        DAT_80022d72 = SUB41(local_10._0_4_,2);
        DAT_80022d73 = SUB41(local_10._0_4_,1);
        DAT_80022d50 = (undefined1)local_10._0_4_;
      }
      goto LAB_800119fc;
    }
    local_18 = local_18 + '\x01';
  } while( true );
}

/* 0x80011a28 FUN_80011a28 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80011a28(void)

{
  ushort uVar1;
  bool bVar2;
  bool bVar3;
  char cVar4;
  ushort uVar5;
  undefined2 uVar6;
  uint uVar7;
  undefined1 *puVar8;
  undefined4 *puVar9;
  uint *puVar10;
  char *pcVar11;
  int iVar12;
  undefined *puVar13;
  int iVar14;
  undefined4 uVar15;
  uint uVar16;
  char cStack_209e8;
  bool bStack_209e7;
  int iStack_209e4;
  ushort uStack_209e0;
  ushort uStack_209d8;
  ushort uStack_209d6;
  char cStack_209d4;
  int iStack_209d0;
  int iStack_209cc;
  char *pcStack_209c4;
  int iStack_209c0;
  undefined1 auStack_20929 [2];
  undefined1 uStack_20927;
  undefined1 auStack_2090d [2];
  undefined1 uStack_2090b;
  undefined1 auStack_208f8 [2];
  undefined1 uStack_208f6;
  undefined1 auStack_20880 [12];
  undefined1 uStack_20874;
  undefined1 uStack_20873;
  undefined2 uStack_20872;
  undefined2 uStack_20870;
  ushort uStack_2086e;
  undefined2 uStack_2086c;
  undefined1 uStack_20868;
  undefined1 uStack_20867;
  undefined2 uStack_20866;
  undefined2 uStack_20864;
  ushort uStack_20862;
  undefined2 uStack_20860;
  undefined1 auStack_2085c [65536];
  int aiStack_1085c [16386];
  undefined1 auStack_854 [20];
  undefined1 auStack_840 [16];
  undefined1 local_830;
  undefined1 auStack_82c [813];
  undefined1 auStack_4ff [1235];
  char acStack_2c [36];
  
  _DAT_bf801114 = 0x100;
  FUN_800174b8();
  FUN_8001622c();
  FUN_80018340();
  FUN_80015a8c();
  FUN_80018bdc(&DAT_8001f130,0x80045024);
  FUN_80018bdc(&DAT_8001f850,0x8004502c);
  FUN_800117f8();
  FUN_800143c4();
  cStack_209e8 = '\x01';
  if ((_DAT_bf801814 & 0x100000) == 0x100000) {
    _ftext(1,0x140,0xf0);
  }
  else {
    _ftext(0,0x140,0xf0);
  }
  _DAT_bf8010f0 = _DAT_bf8010f0 | 0x800;
  _DAT_bf801814 = 0x3000000;
  FUN_800109cc(auStack_20880,&DAT_8001dc30,&DAT_8001ebf0,0x280,0,0x280,0x54,0x60,0x54,0);
  FUN_800109cc(&uStack_20874,&DAT_8001ec10,&DAT_8001f110,0x2e0,0,0x2e0,0x14,0x80,0x14,0);
  bStack_209e7 = false;
  iStack_209e4 = 0;
  uStack_209e0 = 0;
  bVar2 = false;
  uStack_209d8 = FUN_80015efc(0);
  do {
    do {
      if (bStack_209e7 == false) {
        uVar7 = 0;
      }
      else {
        uVar7 = 0x140;
      }
      uVar16 = (uint)bStack_209e7;
      puVar8 = auStack_2085c + uVar16 * 0x10004;
      bStack_209e7 = bStack_209e7 == false;
      _DAT_bf801814 = uVar7 | 0x5000000;
      aiStack_1085c[uVar16 * 0x4001] = (int)puVar8;
      puVar9 = (undefined4 *)FUN_80010668(puVar8,4);
      *puVar9 = 0xe1000200;
      puVar9[1] = uVar7 | 0xe3000000;
      puVar9[2] = uVar7 + 0x13f | 0xe403b800;
      puVar9[3] = uVar7 | 0xe5000000;
      puVar10 = (uint *)FUN_80010668(puVar8,3);
      _auStack_208f8 = CONCAT12(DAT_80044ffc,CONCAT11(DAT_80045021,DAT_80045020));
      *puVar10 = _auStack_208f8 | 0x2000000;
      puVar10[1] = uVar7;
      puVar10[2] = 0xf00140;
      puVar10 = (uint *)FUN_80010668(puVar8,3);
      _auStack_2090d = CONCAT12(DAT_80044fff,CONCAT11(DAT_80044ffe,DAT_80044ffd));
      *puVar10 = _auStack_2090d | 0x60000000;
      puVar10[1] = 0xe0008;
      puVar10[2] = 0xca00ae;
      puVar10 = (uint *)FUN_80010668(puVar8,3);
      _auStack_20929 = CONCAT12(_fdata,CONCAT11(DAT_80045023,DAT_80045022));
      *puVar10 = _auStack_20929 | 0x60000000;
      puVar10[1] = 0xf0009;
      puVar10[2] = 0xc800ac;
      puVar10 = (uint *)FUN_80010668(puVar8,5);
      *puVar10 = uStack_2086e & 0x9ff | 0xe1000000;
      puVar10[1] = 0x67000000;
      puVar10[2] = 0xe00b9;
      puVar10[3] = CONCAT22(uStack_2086c,CONCAT11(uStack_20873,uStack_20874));
      puVar10[4] = CONCAT22(uStack_20870,uStack_20872);
      uVar5 = FUN_80015efc(0);
      uStack_209d6 = ~uStack_209d8 & uVar5;
      if (uVar5 == 0) {
        if ((!bVar2) && (cStack_209e8 == '\0')) {
          if (DAT_80045001 == '\0') {
            if (DAT_80045002 != '\0') {
              DAT_80045002 = DAT_80045002 + -1;
            }
          }
          else {
            DAT_80045001 = DAT_80045001 + -1;
          }
        }
      }
      else {
        DAT_80045001 = '(';
        DAT_80045002 = '\x1e';
        DAT_80045034 = 0;
        DAT_80045003 = '\x01';
        _DAT_80045038 = 0;
        DAT_8004503c = 0;
      }
      if (((uVar5 & 0x10) == 0) || ((uStack_209d8 & 0x10) == 0)) {
        if (((uVar5 & 0x40) == 0) || ((uStack_209d8 & 0x40) == 0)) {
          DAT_8004503d = 0;
        }
        else {
          DAT_8004503d = DAT_8004503d + 1;
          if (0x1e < DAT_8004503d) {
            uStack_209d6 = uStack_209d6 ^ 0x40;
            DAT_8004503d = 0x19;
          }
        }
      }
      else {
        DAT_8004503d = DAT_8004503d + 1;
        if (0x1e < DAT_8004503d) {
          uStack_209d6 = uStack_209d6 ^ 0x10;
          DAT_8004503d = 0x19;
        }
      }
      if ((uStack_209d6 & 1) != 0) {
        bVar2 = (bool)(bVar2 ^ 1);
      }
      if (bVar2) {
        FUN_80011420(puVar8,auStack_20880,0x28,0x28,"PicosSation Menu Alpha Release");
        FUN_80011420(puVar8,auStack_20880,0x28,0x50,
                     "Huge thanks to Rama, Skitchin, Raijin, SpicyJpeg,\nDanhans42, NicholasNoble and ChatGPT"
                    );
        FUN_80011420(puVar8,auStack_20880,0x28,0x78,"https://github.com/megavolt85/picostation-menu"
                    );
      }
      else {
        if (iStack_209e4 != 0) {
          if ((uStack_209d6 & 0x10) == 0) {
            if ((uStack_209d6 & 0x40) != 0) {
              if ((int)(uint)uStack_209e0 < iStack_209e4 + -1) {
                uStack_209e0 = uStack_209e0 + 1;
              }
              else {
                uStack_209e0 = 0;
              }
            }
          }
          else {
            uVar1 = (ushort)iStack_209e4;
            if (uStack_209e0 != 0) {
              uVar1 = uStack_209e0;
            }
            uStack_209e0 = uVar1 - 1;
          }
          if ((uStack_209d6 & 0x480) == 0) {
            if ((uStack_209d6 & 0x820) != 0) {
              if ((int)(uint)uStack_209e0 < iStack_209e4 + -0x13) {
                uStack_209e0 = uStack_209e0 + 0x12;
              }
              else {
                uStack_209e0 = (ushort)iStack_209e4 - 1;
              }
            }
          }
          else if (uStack_209e0 < 0x12) {
            uStack_209e0 = 0;
          }
          else {
            uStack_209e0 = uStack_209e0 - 0x12;
          }
          if ((uStack_209d6 & 0xcf0) != 0) {
            FUN_800189c4(0x80045024,0x2aaa,0x2aaa,0);
          }
          if (((uStack_209d6 & 8) != 0) &&
             (pcVar11 = (char *)FUN_80014508(uStack_209e0), *pcVar11 == '\0')) {
            cStack_209e8 = '\x05';
          }
          if ((uStack_209d6 & 0x4000) != 0) {
            pcVar11 = (char *)FUN_80014508(uStack_209e0);
            if (*pcVar11 == '\0') {
              cStack_209e8 = '\x04';
            }
            else {
              cStack_209e8 = '\x03';
            }
          }
          if ((uStack_209d6 & 0x4008) != 0) {
            FUN_800189c4(0x8004502c,0x2aaa,0x2aaa,1);
          }
        }
        if (((short)uStack_209d6 < 0) && (DAT_8004503e != 0)) {
          cStack_209e8 = '\x02';
          FUN_800189c4(0x8004502c,0x2aaa,0x2aaa,1);
        }
        cStack_209d4 = '\0';
        if (cStack_209e8 == '\0') {
          if (iStack_209e4 == 0) {
            iVar14 = 0;
          }
          else {
            iVar14 = uStack_209e0 + 1;
          }
          FUN_8001bdc0(acStack_2c,0x20,"%i of %i",iVar14,iStack_209e4);
          FUN_80011420(puVar8,auStack_20880,0xba,0x42,acStack_2c);
          iStack_209d0 = 0;
          if (0x11 < iStack_209e4) {
            iVar14 = uStack_209e0 - 9;
            if (iVar14 < 0) {
              iVar14 = 0;
            }
            iStack_209d0 = iStack_209e4 + -0x12;
            if (iVar14 < iStack_209e4 + -0x12) {
              iStack_209d0 = iVar14;
            }
          }
          iVar14 = iStack_209d0 + 0x12;
          if (iStack_209e4 < iStack_209d0 + 0x12) {
            iVar14 = iStack_209e4;
          }
          if (iVar14 - iStack_209d0 < 1) {
            FUN_80011420(puVar8,auStack_20880,0x28,0x28,"Empty Folder");
          }
          else {
            if (_DAT_80045038 == 4) {
              puVar10 = (uint *)FUN_80010668(puVar8,5);
              *puVar10 = uStack_20862 & 0x9ff | 0xe1000000;
              puVar10[1] = 0x67000000;
              puVar10[2] = 0x5200b9;
              puVar10[3] = CONCAT22(uStack_20860,CONCAT11(uStack_20867,uStack_20868));
              puVar10[4] = CONCAT22(uStack_20864,uStack_20866);
            }
            if (DAT_80045001 == '\0') {
              if (_DAT_80045038 == 2) {
                if (DAT_80022dcc == '\x01') {
                  if (DAT_80022dbd != '\x01') {
                    iVar12 = FUN_8001997c(&DAT_80023700,&DAT_8001cf44,4);
                    uVar1 = DAT_80023704;
                    if (iVar12 == 0) {
                      if (DAT_80023704 == DAT_8004503c) {
                        FUN_80019cdc(&DAT_80024018 + (uint)DAT_80023704 * 0x800,&DAT_80023818,0x800)
                        ;
                        if (uVar1 == 0xf) {
                          FUN_8001077c(&uStack_20868,&DAT_80024018,0,0x100,0x80,0x80);
                          _DAT_80045038 = 4;
                          FUN_80010d64(0,0);
                        }
                        else {
                          DAT_8004503c = DAT_8004503c + 1;
                          _DAT_80045038 = 1;
                        }
                      }
                      else {
                        _DAT_80045038 = 1;
                      }
                    }
                    else {
                      _DAT_80045038 = 3;
                      FUN_80010d64(0,0);
                    }
                  }
                }
                else {
                  _DAT_80045038 = 3;
                  FUN_80010d64(0,0);
                }
              }
              else if (_DAT_80045038 < 3) {
                if (_DAT_80045038 == 0) {
                  uVar15 = FUN_80014574(uStack_209e0);
                  FUN_80010d64(9,uVar15);
                  _DAT_80045038 = 1;
                  DAT_8004503c = 0;
                }
                else if (_DAT_80045038 == 1) {
                  FUN_80019c10(&DAT_800236f4,0,0x20);
                  FUN_8001657c(DAT_8004503c + 200,&DAT_800236f4,1,0x924,1,0);
                  _DAT_80045038 = 2;
                }
              }
            }
            for (iStack_209cc = 0; iStack_209cc < iVar14 - iStack_209d0;
                iStack_209cc = iStack_209cc + 1) {
              pcVar11 = (char *)FUN_80014508(iStack_209d0 + iStack_209cc & 0xffff);
              if (*pcVar11 == '\0') {
                uVar15 = 0xffffff8f;
              }
              else {
                uVar15 = 0xffffff92;
              }
              FUN_8001bdc0(auStack_82c,299,"%c %s",uVar15,pcVar11 + 1);
              if (iStack_209d0 + iStack_209cc == (uint)uStack_209e0) {
                cStack_209d4 = *pcVar11;
                iVar12 = FUN_80010e00(auStack_82c);
                uVar7 = iVar12 + 2;
                if (0xae < (int)uVar7) {
                  if (DAT_80045002 == '\0') {
                    DAT_80045002 = '\x0f';
                    if ((DAT_80045034 == 0) && (DAT_80045003 == -1)) {
                      DAT_80045003 = '\x01';
                      DAT_80045002 = '\x1e';
                    }
                    else {
                      DAT_80045034 = DAT_80045034 + DAT_80045003;
                    }
                  }
                  if (*pcVar11 == '\0') {
                    uVar15 = 0xffffff8f;
                  }
                  else {
                    uVar15 = 0xffffff92;
                  }
                  FUN_8001bdc0(auStack_82c,299,"%c %s",uVar15,pcVar11 + DAT_80045034 + 1);
                  iVar12 = FUN_80010e00(auStack_82c);
                  if ((iVar12 < 0xa8) && ('\0' < DAT_80045003)) {
                    DAT_80045003 = -1;
                    DAT_80045002 = '\x1e';
                  }
                }
                puVar10 = (uint *)FUN_80010668(puVar8,3);
                *puVar10 = DAT_80045004 | 0x60000000;
                puVar10[1] = (iStack_209cc * 0xb + 0xf) * 0x10000 | 9;
                if (0xac < (int)uVar7) {
                  uVar7 = 0xac;
                }
                puVar10[2] = uVar7 & 0xffff | 0xc0000;
              }
              FUN_80010f84(puVar8,auStack_20880,10,iStack_209cc * 0xb + 0x11,auStack_82c,0xb4);
            }
          }
          if (iStack_209e4 == 0) {
            if (DAT_8004503e == 0) {
              puVar13 = &DAT_8001cf64;
            }
            else {
              puVar13 = &DAT_8001cf68;
            }
            FUN_8001bdc0(auStack_82c,299," \t\t \t\t \t\t \t\t \t\t \t\t%s",puVar13);
          }
          else if (cStack_209d4 == '\0') {
            if (DAT_8004503e == 0) {
              puVar13 = &DAT_8001cf64;
            }
            else {
              puVar13 = &DAT_8001cf68;
            }
            FUN_8001bdc0(auStack_82c,299,&DAT_8001cf90,puVar13);
          }
          else {
            if (DAT_8004503e == 0) {
              puVar13 = &DAT_8001cf64;
            }
            else {
              puVar13 = &DAT_8001cf68;
            }
            FUN_8001bdc0(auStack_82c,299,&DAT_8001cfb0,puVar13);
          }
          FUN_80011420(puVar8,auStack_20880,0xc,0xda,auStack_82c);
          DAT_80045004 = (char)_DAT_80045008 + DAT_80045004;
          if ((char)DAT_80045004 < '\0') {
            _DAT_80045008 = 0xffffffff;
          }
          else if (DAT_80045004 < 0x30) {
            _DAT_80045008 = 1;
          }
        }
        else {
          FUN_80011420(puVar8,auStack_20880,0x28,0x28,"Please Wait Loading...");
        }
      }
      *(undefined4 *)aiStack_1085c[uVar16 * 0x4001] = 0xffffff;
      FUN_8001034c();
      FUN_80017518();
      FUN_800103e4(puVar8);
      uStack_209d8 = uVar5;
    } while (cStack_209e8 == '\0');
    if (cStack_209e8 == '\x01') {
      iStack_209e4 = FUN_800116e4(&DAT_80022dd0,1,0);
    }
    else if (cStack_209e8 == '\x02') {
      iStack_209e4 = FUN_800116e4(&DAT_80022dd0,2,0);
      if (DAT_8004503e != 0) {
        DAT_8004503e = DAT_8004503e - 1;
      }
      uStack_209e0 = *(ushort *)(&DAT_8002c018 + (uint)DAT_8004503e * 2);
    }
    else if (cStack_209e8 == '\x03') {
      uVar6 = FUN_80014574(uStack_209e0);
      iStack_209e4 = FUN_800116e4(&DAT_80022dd0,3,uVar6);
      uVar7 = (uint)DAT_8004503e;
      DAT_8004503e = DAT_8004503e + 1;
      *(ushort *)(&DAT_8002c018 + uVar7 * 2) = uStack_209e0;
      uStack_209e0 = 0;
    }
    else if ((cStack_209e8 == '\x04') || (cStack_209e8 == '\x05')) {
      uVar6 = FUN_80014574(uStack_209e0);
      FUN_80010d64(5,uVar6);
      FUN_80016c24(400000);
      FUN_80016908();
      FUN_80016c24(400000);
      iVar14 = FUN_80016964();
      if (iVar14 == 0) {
        cStack_209e8 = '\x05';
      }
      else {
        bVar3 = false;
        iVar14 = FUN_80016c94();
        if (iVar14 == 0) {
          pcStack_209c4 = "cdrom:PS.EXE;1";
          iVar14 = FUN_80016b90("SYSTEM.CNF;1",auStack_82c);
          if ((iVar14 == 0) &&
             (iVar14 = FUN_8001487c(auStack_82c,&DAT_8001d090,acStack_2c), iVar14 != 0)) {
            pcStack_209c4 = acStack_2c;
          }
          iVar14 = FUN_800198cc(pcStack_209c4,"cdrom:\\psx.exe");
          if ((((iVar14 == 0) ||
               (iVar14 = FUN_800198cc(pcStack_209c4,"cdrom:\\PSX.EXE;1"), iVar14 == 0)) ||
              (iVar14 = FUN_800198cc(pcStack_209c4,"cdrom:\\psx.exe;1"), iVar14 == 0)) ||
             (iVar14 = FUN_800198cc(pcStack_209c4,"cdrom:PSX.EXE;1"), iVar14 == 0)) {
            bVar3 = true;
          }
          cVar4 = FUN_80019ba4(pcStack_209c4);
          if (bVar3) {
            FUN_8001657c(0x10,auStack_82c,1,0x800,1,1);
            for (iStack_209c0 = 0; iStack_209c0 < 0x10; iStack_209c0 = iStack_209c0 + 1) {
              auStack_840[iStack_209c0] = auStack_4ff[iStack_209c0];
            }
            local_830 = 0;
            uVar15 = FUN_800149c4(auStack_840);
            if (DAT_80022d8f == '\0') {
              FUN_8001bd50(auStack_854,"%s%s%s%s","cdrom:","SLPS_",uVar15,&DAT_8001d0f8);
            }
            else {
              FUN_8001bd50(auStack_854,"%s%s%s%s","cdrom:","SCPS_",uVar15,&DAT_8001d0f8);
            }
            FUN_800198cc(uVar15,&DAT_8001d108);
          }
          else if (cVar4 < '\x14') {
            iVar14 = FUN_80019af4(pcStack_209c4,&DAT_8001d0f8);
            if (iVar14 == 0) {
              FUN_8001bd50(auStack_854,&DAT_8001d10c,pcStack_209c4,&DAT_8001d0f8);
            }
            else {
              FUN_80019758(auStack_854,pcStack_209c4);
            }
          }
          else {
            FUN_8001bd50(auStack_854,&DAT_8001d10c,"cdrom:",pcStack_209c4 + cVar4 + -0xd);
          }
          FUN_8001614c(auStack_854);
        }
      }
      if (cStack_209e8 == '\x04') {
        FUN_800178c8();
      }
      else {
        FUN_800178a4();
      }
    }
    cStack_209e8 = '\0';
  } while( true );
}

/* 0x80013d58 FUN_80013d58 */
undefined4 FUN_80013d58(ushort param_1,ushort param_2)

{
  char *pcVar1;
  char *pcVar2;
  undefined4 uVar3;
  
  pcVar1 = (char *)(DAT_80022db0 + (uint)param_1 * 0x101);
  pcVar2 = (char *)(DAT_80022db0 + (uint)param_2 * 0x101);
  if ((*pcVar1 == '\x01') && (*pcVar2 == '\0')) {
    uVar3 = 0xffffffff;
  }
  else if ((*pcVar1 == '\0') && (*pcVar2 == '\x01')) {
    uVar3 = 1;
  }
  else {
    uVar3 = FUN_800198cc(pcVar1 + 1,pcVar2 + 1);
  }
  return uVar3;
}

/* 0x80013e7c FUN_80013e7c */
void FUN_80013e7c(ushort param_1,ushort param_2)

{
  undefined2 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 local_18;
  undefined4 local_14;
  
  if (param_1 < param_2) {
    uVar1 = *(undefined2 *)(DAT_80022dac + (((uint)param_1 + (uint)param_2) / 2) * 2);
    local_18 = (uint)param_1;
    local_14 = (uint)param_2;
    while ((int)local_18 <= (int)local_14) {
      while (iVar3 = FUN_80013d58(*(undefined2 *)(DAT_80022dac + local_18 * 2),uVar1), iVar3 < 0) {
        local_18 = local_18 + 1;
      }
      while (iVar3 = FUN_80013d58(*(undefined2 *)(DAT_80022dac + local_14 * 2),uVar1), 0 < iVar3) {
        local_14 = local_14 - 1;
      }
      if ((int)local_18 <= (int)local_14) {
        uVar2 = *(undefined2 *)(DAT_80022dac + local_18 * 2);
        *(undefined2 *)(DAT_80022dac + local_18 * 2) = *(undefined2 *)(DAT_80022dac + local_14 * 2);
        *(undefined2 *)(DAT_80022dac + local_14 * 2) = uVar2;
        local_18 = local_18 + 1;
        local_14 = local_14 - 1;
      }
    }
    if ((int)(uint)param_1 < (int)local_14) {
      FUN_80013e7c(param_1,local_14 & 0xffff);
    }
    if ((int)local_18 < (int)(uint)param_2) {
      FUN_80013e7c(local_18 & 0xffff,param_2);
    }
  }
  return;
}

/* 0x80014124 FUN_80014124 */
void FUN_80014124(ushort *param_1)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  int local_28;
  int local_24;
  
  local_28 = 0;
LAB_8001437c:
  do {
    if ((int)(*param_1 - 1) <= local_28) {
      return;
    }
    iVar1 = DAT_80022db0 + (uint)*(ushort *)(DAT_80022dac + local_28 * 2) * 0x101 + 1;
    uVar2 = FUN_80019ba4(iVar1);
    if ((3 < uVar2) && (iVar3 = FUN_800198cc(iVar1 + (uVar2 - 4),&DAT_8001d120), iVar3 == 0)) {
      iVar3 = DAT_80022db0 + (uint)*(ushort *)(DAT_80022dac + (local_28 + 1) * 2) * 0x101 + 1;
      uVar4 = FUN_80019ba4(iVar3);
      if ((uVar2 == uVar4) &&
         ((iVar1 = FUN_8001997c(iVar1,iVar3,uVar2 - 4), iVar1 == 0 &&
          (iVar1 = FUN_800198cc(iVar3 + (uVar2 - 4),&DAT_8001d128), iVar1 == 0)))) {
        for (local_24 = local_28; local_24 < (int)(*param_1 - 1); local_24 = local_24 + 1) {
          *(undefined2 *)(DAT_80022dac + local_24 * 2) =
               *(undefined2 *)(DAT_80022dac + (local_24 + 1) * 2);
        }
        *param_1 = *param_1 - 1;
        goto LAB_8001437c;
      }
    }
    local_28 = local_28 + 1;
  } while( true );
}

/* 0x800143c4 FUN_800143c4 */
void FUN_800143c4(void)

{
  DAT_80022dac = FUN_80018ff0(0x2000);
  DAT_80022db0 = FUN_80018ff0(0x101000);
  return;
}

/* 0x80014424 FUN_80014424 */
void FUN_80014424(ushort param_1,undefined1 param_2,undefined4 param_3,ushort param_4)

{
  undefined1 *puVar1;
  
  puVar1 = (undefined1 *)(DAT_80022db0 + (uint)param_1 * 0x101);
  *puVar1 = param_2;
  FUN_80019cdc(puVar1 + 1,param_3,param_4);
  puVar1[param_4 + 1] = 0;
  *(ushort *)(DAT_80022dac + (uint)param_1 * 2) = param_1;
  return;
}

/* 0x80014508 FUN_80014508 */
int FUN_80014508(ushort param_1)

{
  return DAT_80022db0 + (uint)*(ushort *)(DAT_80022dac + (uint)param_1 * 2) * 0x101;
}

/* 0x80014574 FUN_80014574 */
undefined2 FUN_80014574(ushort param_1)

{
  return *(undefined2 *)(DAT_80022dac + (uint)param_1 * 2);
}

/* 0x800145b8 FUN_800145b8 */
void FUN_800145b8(short param_1)

{
  FUN_80013e7c(0,(param_1 + -1) * 0x10000 >> 0x10 & 0xffff);
  return;
}

/* 0x80014614 FUN_80014614 */
int FUN_80014614(char param_1)

{
  int iVar1;
  
  iVar1 = FUN_800195c8((int)param_1);
  if (iVar1 == 0) {
    iVar1 = FUN_80019544((int)param_1);
    if (iVar1 == 0) {
      iVar1 = 0;
    }
    else {
      iVar1 = FUN_80019624((int)param_1);
      iVar1 = iVar1 + -0x41;
    }
  }
  else {
    iVar1 = param_1 + -0x30;
  }
  return iVar1;
}

/* 0x800146b4 FUN_800146b4 */
int FUN_800146b4(int param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int local_res0;
  
  iVar1 = FUN_80019ba4(param_2);
  local_res0 = param_1;
  while( true ) {
    iVar2 = FUN_8001997c(local_res0,param_2,iVar1);
    if (iVar2 == 0) {
      return local_res0 + iVar1;
    }
    local_res0 = FUN_80019a68(local_res0,10);
    if (local_res0 == 0) break;
    local_res0 = local_res0 + 1;
  }
  return 0;
}

/* 0x80014768 FUN_80014768 */
undefined4 FUN_80014768(undefined4 param_1,undefined4 param_2,uint *param_3)

{
  undefined4 uVar1;
  uint uVar2;
  char *local_res0;
  uint local_10;
  
  local_res0 = (char *)FUN_800146b4(param_1,param_2);
  if (local_res0 == (char *)0x0) {
    uVar1 = 0;
  }
  else {
    local_10 = 0;
    for (; (*local_res0 != '\n' && (*local_res0 != '\0')); local_res0 = local_res0 + 1) {
      uVar2 = FUN_80014614((int)*local_res0);
      if (uVar2 < 0x10) {
        local_10 = uVar2 | local_10 << 4;
      }
    }
    *param_3 = local_10;
    uVar1 = 1;
  }
  return uVar1;
}

/* 0x8001487c FUN_8001487c */
undefined4 FUN_8001487c(undefined4 param_1,undefined4 param_2,char *param_3)

{
  undefined4 uVar1;
  int iVar2;
  char *local_res0;
  char *local_10;
  
  local_res0 = (char *)FUN_800146b4(param_1,param_2);
  if (local_res0 == (char *)0x0) {
    uVar1 = 0;
  }
  else {
    while ((*local_res0 == '=' ||
           (iVar2 = FUN_800194d8((int)*local_res0), local_10 = param_3, iVar2 != 0))) {
      local_res0 = local_res0 + 1;
    }
    while ((*local_res0 != '\0' && (iVar2 = FUN_800194d8((int)*local_res0), iVar2 == 0))) {
      *local_10 = *local_res0;
      local_res0 = local_res0 + 1;
      local_10 = local_10 + 1;
    }
    *local_10 = '\0';
    uVar1 = 1;
  }
  return uVar1;
}

/* 0x800149c4 FUN_800149c4 */
char * FUN_800149c4(undefined4 param_1)

{
  int iVar1;
  char *pcVar2;
  
  iVar1 = FUN_800198cc(param_1,"1994111009000000");
  if (iVar1 == 0) {
    pcVar2 = "000.01";
  }
  else {
    iVar1 = FUN_800198cc(param_1,"1994110702000000");
    if (iVar1 == 0) {
      pcVar2 = "000.02";
    }
    else {
      iVar1 = FUN_800198cc(param_1,"1994102615231700");
      if (iVar1 == 0) {
        pcVar2 = "000.03";
      }
      else {
        iVar1 = FUN_800198cc(param_1,"1994110218594700");
        if ((iVar1 == 0) || (iVar1 = FUN_800198cc(param_1,"1995030218052000"), iVar1 == 0)) {
          pcVar2 = "000.04";
        }
        else {
          iVar1 = FUN_800198cc(param_1,"1994110722360400");
          if ((iVar1 == 0) || (iVar1 = FUN_800198cc(param_1,"1994120610494900"), iVar1 == 0)) {
            pcVar2 = "000.05";
          }
          else {
            iVar1 = FUN_800198cc(param_1,"1994110407000000");
            if (iVar1 == 0) {
              pcVar2 = "000.06";
            }
            else {
              iVar1 = FUN_800198cc(param_1,"1994111419300000");
              if (iVar1 == 0) {
                pcVar2 = "000.07";
              }
              else {
                iVar1 = FUN_800198cc(param_1,"1994121808190700");
                if (iVar1 == 0) {
                  pcVar2 = "000.08";
                }
                else {
                  iVar1 = FUN_800198cc(param_1,"1994121917000000");
                  if (iVar1 == 0) {
                    pcVar2 = "000.09";
                  }
                  else {
                    iVar1 = FUN_800198cc(param_1,"1995052918000000");
                    if (iVar1 == 0) {
                      pcVar2 = "000.10";
                    }
                    else {
                      iVar1 = FUN_800198cc(param_1,"1994110220020600");
                      if (iVar1 == 0) {
                        pcVar2 = "000.11";
                      }
                      else {
                        iVar1 = FUN_800198cc(param_1,"1994121518000000");
                        if (iVar1 == 0) {
                          pcVar2 = "000.13";
                        }
                        else {
                          iVar1 = FUN_800198cc(param_1,"1994103000000000");
                          if (iVar1 == 0) {
                            pcVar2 = "000.14";
                          }
                          else {
                            iVar1 = FUN_800198cc(param_1,"1994101813262400");
                            if (iVar1 == 0) {
                              pcVar2 = "000.15";
                            }
                            else {
                              iVar1 = FUN_800198cc(param_1,"1994112617300000");
                              if ((iVar1 == 0) ||
                                 (iVar1 = FUN_800198cc(param_1,"1994121517300000"), iVar1 == 0)) {
                                pcVar2 = "000.16";
                              }
                              else {
                                iVar1 = FUN_800198cc(param_1,"1994111013000000");
                                if (iVar1 == 0) {
                                  pcVar2 = "000.17";
                                }
                                else {
                                  iVar1 = FUN_800198cc(param_1,"1994111522183200");
                                  if (iVar1 == 0) {
                                    pcVar2 = "000.18";
                                  }
                                  else {
                                    iVar1 = FUN_800198cc(param_1,"1994112918000000");
                                    if (iVar1 == 0) {
                                      pcVar2 = "000.19";
                                    }
                                    else {
                                      iVar1 = FUN_800198cc(param_1,"1994111721302100");
                                      if (iVar1 == 0) {
                                        pcVar2 = "000.20";
                                      }
                                      else {
                                        iVar1 = FUN_800198cc(param_1,"1994100617242100");
                                        if (iVar1 == 0) {
                                          pcVar2 = "000.21";
                                        }
                                        else {
                                          iVar1 = FUN_800198cc(param_1,"1995030215000000");
                                          if (iVar1 == 0) {
                                            pcVar2 = "000.22";
                                          }
                                          else {
                                            iVar1 = FUN_800198cc(param_1,"1994122718351900");
                                            if (iVar1 == 0) {
                                              pcVar2 = "000.23";
                                            }
                                            else {
                                              iVar1 = FUN_800198cc(param_1,"1994092920284600");
                                              if (iVar1 == 0) {
                                                pcVar2 = "000.24";
                                              }
                                              else {
                                                iVar1 = FUN_800198cc(param_1,"1994113012000000");
                                                if ((iVar1 == 0) ||
                                                   (iVar1 = FUN_800198cc(param_1,"1995012512000000")
                                                   , iVar1 == 0)) {
                                                  pcVar2 = "000.25";
                                                }
                                                else {
                                                  iVar1 = FUN_800198cc(param_1,"1995041921063500");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.26";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1994121500000000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.27";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1994121017582300");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.28";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995022623000000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.29";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995050116000000");
                                                  if ((iVar1 == 0) ||
                                                     (iVar1 = FUN_800198cc(param_1,
                                                  "1995060613000000"), iVar1 == 0)) {
                                                    pcVar2 = "000.30";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995021802000000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.31";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995021615022900");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.32";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995100209000000")
                                                    ;
                                                    if ((iVar1 == 0) ||
                                                       (iVar1 = FUN_800198cc(param_1,
                                                  "1995080809000000"), iVar1 == 0)) {
                                                    pcVar2 = "000.33";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995071821394900")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.34";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995042506300000");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.35";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995011411551700")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.37";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995041311392800");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.38";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995031205000000")
                                                    ;
                                                    if ((iVar1 == 0) ||
                                                       (iVar1 = FUN_800198cc(param_1,
                                                  "1995061612000000"), iVar1 == 0)) {
                                                    pcVar2 = "000.40";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995040509000000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.41";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995052612000000");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.43";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995042500000000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.44";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995033100003000");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.47";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995041400000000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.48";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995050413421800");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.50";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995040509595900")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.51";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995030103150000");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.52";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995100409235300")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.53";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995060504013600");
                                                  if (((iVar1 == 0) ||
                                                      (iVar1 = FUN_800198cc(param_1,
                                                  "1995060319142200"), iVar1 == 0)) ||
                                                  (iVar1 = FUN_800198cc(param_1,"1995060402110800"),
                                                  iVar1 == 0)) {
                                                    pcVar2 = "000.55";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995081612000000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.59";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995051201000000");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.60";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995051700000000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.61";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995051002471900");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.63";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995083112000000")
                                                    ;
                                                    if ((iVar1 == 0) ||
                                                       (iVar1 = FUN_800198cc(param_1,
                                                  "1995111700000000"), iVar1 == 0)) {
                                                    pcVar2 = "000.64";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1996033100000000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.65";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995051816000000");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.66";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995061418000000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.67";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995061911303400");
                                                  if ((iVar1 == 0) ||
                                                     (iVar1 = FUN_800198cc(param_1,
                                                  "1995072800300000"), iVar1 == 0)) {
                                                    pcVar2 = "000.68";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995061806364400")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.69";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995062922000000");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.70";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995040719355400")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.71";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995061806364400");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.73";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995051015300000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.77";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995070302000000");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.78";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995070523450000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.83";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995072522004900");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.85";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995070613170000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.88";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995082517551900");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.89";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995082109402500")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.90";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995053117000000");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.91";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995081100000000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.92";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995071011035200");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "000.93";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995090510000000")
                                                    ;
                                                    if ((iVar1 == 0) ||
                                                       (iVar1 = FUN_800198cc(param_1,
                                                  "1995083123000000"), iVar1 == 0)) {
                                                    pcVar2 = "000.94";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995100601300000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "000.99";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995081001450000");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "001.01";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995080316000000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "001.03";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995081020000000");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "001.04";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995090722000000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "001.08";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995090516062841");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "001.13";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995082016003000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "001.28";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995102101350000");
                                                  if (((iVar1 == 0) ||
                                                      (iVar1 = FUN_800198cc(param_1,
                                                  "1995102102521200"), iVar1 == 0)) ||
                                                  (iVar1 = FUN_800198cc(param_1,"1995102105003200"),
                                                  iVar1 == 0)) {
                                                    pcVar2 = "001.33";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995100910002200")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "001.37";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995101801325900");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "001.42";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995113010450000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "001.46";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995092205430500");
                                                  if (iVar1 == 0) {
                                                    pcVar2 = "001.52";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995121620000000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      pcVar2 = "001.73";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995030717020700");
                                                  if (iVar1 == 0) {
                                                    DAT_80022d8f = 1;
                                                    pcVar2 = "100.02";
                                                  }
                                                  else {
                                                    iVar1 = FUN_800198cc(param_1,"1995032400000000")
                                                    ;
                                                    if (iVar1 == 0) {
                                                      DAT_80022d8f = 1;
                                                      pcVar2 = "100.07";
                                                    }
                                                    else {
                                                      iVar1 = FUN_800198cc(param_1,
                                                  "1995052420065100");
                                                  if (iVar1 == 0) {
                                                    DAT_80022d8f = 1;
                                                    pcVar2 = "100.08";
                                                  }
                                                  else {
                                                    pcVar2 = "0";
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                  }
                                                }
                                              }
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return pcVar2;
}

/* 0x80015a8c FUN_80015a8c */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80015a8c(void)

{
  _DAT_bf801048 = 0xd;
  _DAT_bf80104e = 0x87;
  _DAT_bf80104a = 0x1005;
  return;
}

/* 0x80015af0 FUN_80015af0 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_80015af0(int param_1)

{
  int local_res0;
  
  local_res0 = param_1;
  while( true ) {
    if (local_res0 < 1) {
      return 0;
    }
    if ((_DAT_bf801070 & 0x80) != 0) break;
    FUN_80016c24(10);
    local_res0 = local_res0 + -10;
  }
  _DAT_bf80104a = _DAT_bf80104a | 0x10;
  _DAT_bf801070 = 0xff7f;
  return 1;
}

/* 0x80015bb8 FUN_80015bb8 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80015bb8(int param_1)

{
  if (param_1 == 0) {
    _DAT_bf80104a = _DAT_bf80104a & 0xdfff;
  }
  else {
    _DAT_bf80104a = _DAT_bf80104a | 0x2000;
  }
  return;
}

/* 0x80015c4c FUN_80015c4c */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined1 FUN_80015c4c(undefined1 param_1)

{
  do {
  } while ((_DAT_bf801044 & 1) == 0);
  DAT_bf801040 = param_1;
  do {
  } while ((_DAT_bf801044 & 2) == 0);
  return param_1;
}

/* 0x80015cec FUN_80015cec */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int FUN_80015cec(undefined1 param_1,undefined1 *param_2,undefined1 *param_3,int param_4,int param_5)

{
  undefined1 uVar1;
  char cVar2;
  int iVar3;
  undefined1 *local_res4;
  undefined1 *local_res8;
  int local_resc;
  int local_18;
  
  FUN_80016c24(10);
  _DAT_bf801070 = 0xff7f;
  _DAT_bf80104a = _DAT_bf80104a | 0x12;
  FUN_80016c24(0x96);
  local_18 = 0;
  DAT_bf801040 = param_1;
  iVar3 = FUN_80015af0(0x78);
  if (iVar3 != 0) {
    do {
      local_res4 = param_2;
      local_res8 = param_3;
      local_resc = param_4;
    } while ((_DAT_bf801044 & 2) != 0);
    do {
      if (param_5 <= local_18) break;
      if (local_resc < 1) {
        uVar1 = FUN_80015c4c(0);
        *local_res8 = uVar1;
      }
      else {
        uVar1 = FUN_80015c4c(*local_res4);
        *local_res8 = uVar1;
        local_resc = local_resc + -1;
        local_res4 = local_res4 + 1;
      }
      local_res8 = local_res8 + 1;
      local_18 = local_18 + 1;
      cVar2 = FUN_80015af0(0x78);
    } while (cVar2 == '\x01');
  }
  FUN_80016c24(0x96);
  _DAT_bf80104a = _DAT_bf80104a & 0xfffd;
  FUN_80016c24(10);
  return local_18;
}

/* 0x80015efc FUN_80015efc */
ushort FUN_80015efc(undefined4 param_1)

{
  ushort uVar1;
  int iVar2;
  undefined1 local_30;
  undefined1 local_2f;
  undefined1 local_2e;
  undefined1 local_2d;
  undefined1 uStack_2c;
  char local_2b;
  byte local_2a;
  byte local_29;
  
  local_30 = 0x42;
  local_2f = 0;
  local_2e = 0;
  local_2d = 0;
  FUN_80015bb8(param_1);
  iVar2 = FUN_80015cec(1,&local_30,&uStack_2c,4,0x20);
  if ((iVar2 < 4) || (local_2b != 'Z')) {
    uVar1 = 0;
  }
  else {
    uVar1 = ~((ushort)local_2a | (ushort)(((uint)local_29 << 0x18) >> 0x10));
  }
  return uVar1;
}

/* 0x80015fe8 FUN_80015fe8 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80015fe8(undefined1 param_1,undefined1 *param_2,int param_3)

{
  undefined1 *local_res4;
  int local_res8;
  
  _DAT_bf801070 = 0xff7f;
  _DAT_bf80104a = _DAT_bf80104a | 0x12;
  FUN_80016c24(0x96);
  DAT_bf801040 = param_1;
  FUN_80016c24(0x1e);
  do {
    local_res4 = param_2;
    local_res8 = param_3;
  } while ((_DAT_bf801044 & 2) != 0);
  for (; 0 < local_res8; local_res8 = local_res8 + -1) {
    FUN_80015c4c(*local_res4);
    FUN_80016c24(0x1e);
    local_res4 = local_res4 + 1;
  }
  FUN_80016c24(0x96);
  _DAT_bf80104a = _DAT_bf80104a & 0xfffd;
  return;
}

/* 0x8001614c FUN_8001614c */
void FUN_8001614c(undefined4 param_1)

{
  int iVar1;
  int local_50;
  undefined1 local_48;
  undefined1 local_47;
  undefined1 local_46;
  undefined1 auStack_45 [61];
  
  iVar1 = FUN_80019ba4(param_1);
  local_48 = 0x21;
  local_47 = 0;
  local_46 = (undefined1)(iVar1 + 1);
  FUN_800197e8(auStack_45,param_1,iVar1 + 1);
  for (local_50 = 0; local_50 < 2; local_50 = local_50 + 1) {
    FUN_80015bb8(local_50);
    FUN_80015fe8(0x81,&local_48,iVar1 + 4);
  }
  return;
}

/* 0x8001622c FUN_8001622c */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8001622c(void)

{
  _DAT_bf801018 = 0x20943;
  _DAT_bf8010f0 = _DAT_bf8010f0 | 0x8000;
  DAT_bf801800 = 3;
  DAT_bf801801 = 0x80;
  DAT_bf801802 = 0;
  DAT_bf801803 = 0x20;
  return;
}

/* 0x80016330 FUN_80016330 */
void FUN_80016330(undefined1 param_1,undefined1 *param_2,int param_3)

{
  bool bVar1;
  int iVar2;
  undefined1 *local_res4;
  int local_res8;
  
  DAT_80022dbd = 1;
  DAT_80022dcd = 1;
  DAT_80022dce = 1;
  DAT_80022dbe = 1;
  DAT_80022dcc = 1;
  DAT_80022dc4 = 0;
  DAT_80022db4 = 0;
  do {
  } while (DAT_bf801800 < '\0');
  DAT_bf801800 = '\x01';
  DAT_bf801803 = 0x40;
  FUN_80016c24(100);
  iVar2 = 0x66;
  do {
    bVar1 = 0 < iVar2;
    iVar2 = iVar2 + -2;
  } while (bVar1);
  do {
  } while (DAT_bf801800 < '\0');
  DAT_bf801800 = 0;
  local_res4 = param_2;
  for (local_res8 = param_3; local_res8 != 0; local_res8 = local_res8 + -1) {
    DAT_bf801802 = *local_res4;
    local_res4 = local_res4 + 1;
  }
  DAT_bf801801 = param_1;
  return;
}

/* 0x800164c4 FUN_800164c4 */
void FUN_800164c4(void)

{
  do {
    if (DAT_80022dcd == '\0') {
      return;
    }
  } while (DAT_80022dcc != '\0');
  return;
}

/* 0x80016520 FUN_80016520 */
void FUN_80016520(void)

{
  do {
    if (DAT_80022dce == '\0') {
      return;
    }
  } while (DAT_80022dcc != '\0');
  return;
}

/* 0x8001657c FUN_8001657c */
void FUN_8001657c(int param_1,undefined4 param_2,undefined4 param_3,int param_4,char param_5,
                 char param_6)

{
  byte bVar1;
  uint uVar2;
  byte local_d;
  undefined1 local_c;
  undefined1 local_b;
  undefined1 local_a;
  
  while (DAT_80045040 != '\0') {
    FUN_80016c24(100);
  }
  DAT_80045040 = 1;
  local_d = 0;
  if (param_4 == 0x924) {
    local_d = 0x20;
  }
  if (param_5 != '\0') {
    local_d = local_d | 0x80;
  }
  uVar2 = param_1 + 0x96;
  bVar1 = (byte)(uVar2 / 0x1194);
  local_c = (undefined1)
            ((uint)((((char)(bVar1 / 10) * 0x6000000 >> 0x18) + (int)(char)bVar1) * 0x1000000) >>
            0x18);
  bVar1 = (byte)((uVar2 / 0x4b) % 0x3c);
  local_b = (undefined1)
            ((uint)((((char)(bVar1 / 10) * 0x6000000 >> 0x18) + (int)(char)bVar1) * 0x1000000) >>
            0x18);
  bVar1 = (byte)(uVar2 % 0x4b);
  local_a = (undefined1)
            ((uint)((((char)(bVar1 / 10) * 0x6000000 >> 0x18) + (int)(char)bVar1) * 0x1000000) >>
            0x18);
  DAT_80022db8 = param_4;
  DAT_80022dc0 = param_3;
  DAT_80022dc8 = param_2;
  FUN_80016c24(100);
  FUN_80016330(0xe,&local_d,1);
  FUN_80016520();
  FUN_80016330(2,&local_c,3);
  FUN_80016520();
  FUN_80016330(6,0,0);
  FUN_80016520();
  if (DAT_80022dcc == '\x01') {
    if (param_6 != '\0') {
      do {
        if (DAT_80022dbd == '\0') {
          return;
        }
        FUN_80016c24(100);
      } while (DAT_80022dcc == '\x01');
      DAT_80045040 = 0;
    }
  }
  else {
    DAT_80045040 = 0;
  }
  return;
}

/* 0x80016908 FUN_80016908 */
void FUN_80016908(void)

{
  undefined1 local_10 [8];
  
  local_10[0] = 1;
  FUN_80016330(0x12,local_10,1);
  FUN_80016520();
  FUN_800164c4();
  return;
}

/* 0x80016964 FUN_80016964 */
bool FUN_80016964(void)

{
  int iVar1;
  undefined1 auStack_808 [8];
  undefined1 auStack_800 [2040];
  
  FUN_8001657c(0x10,auStack_808,1,0x800,1,1);
  iVar1 = FUN_80019684(auStack_800,"PLAYSTATION",0xb);
  return iVar1 == 0;
}

/* 0x800169ec FUN_800169ec */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_800169ec(void)

{
  _DAT_bf8010b0 = DAT_80022dc8;
  _DAT_bf8010b4 = DAT_80022db8 >> 2;
  _DAT_bf8010b8 = 0x11000000;
  DAT_80022dc8 = DAT_80022dc8 + DAT_80022db8;
  DAT_80022dc0 = DAT_80022dc0 + -1;
  if (DAT_80022dc0 == 0) {
    FUN_80016330(9,0,0);
  }
  DAT_80022dbd = 0;
  DAT_80022d90 = 0;
  return;
}

/* 0x80016ac0 FUN_80016ac0 */
void FUN_80016ac0(void)

{
  DAT_80022dcd = 0;
  DAT_80022db4 = 1;
  return;
}

/* 0x80016af8 FUN_80016af8 */
void FUN_80016af8(void)

{
  DAT_80022dc4 = DAT_8002c2b8;
  DAT_80022dce = 0;
  return;
}

/* 0x80016b34 FUN_80016b34 */
void FUN_80016b34(void)

{
  DAT_80022dbe = 0;
  return;
}

/* 0x80016b60 FUN_80016b60 */
void FUN_80016b60(void)

{
  DAT_80022dcc = 0;
  DAT_80045040 = 0;
  return;
}

/* 0x80016b90 FUN_80016b90 */
bool FUN_80016b90(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = FUN_80017114(param_1);
  if (iVar1 != 0) {
    FUN_8001657c(iVar1,param_2,1,0x800,1,1);
  }
  return iVar1 == 0;
}

/* 0x80016c24 FUN_80016c24 */
void FUN_80016c24(int param_1)

{
  bool bVar1;
  int iVar2;
  
  iVar2 = param_1 * 0x10f + 4;
  if (iVar2 < 0) {
    iVar2 = param_1 * 0x10f + 0xb;
  }
  iVar2 = iVar2 >> 3;
  do {
    bVar1 = 0 < iVar2;
    iVar2 = iVar2 + -2;
  } while (bVar1);
  return;
}

/* 0x80016c94 FUN_80016c94 */
void FUN_80016c94(void)

{
  FUN_80017044(&DAT_8002c2c8);
  return;
}

/* 0x80016ccc FUN_80016ccc */
undefined4 FUN_80016ccc(int param_1,undefined4 *param_2)

{
  *param_2 = *(undefined4 *)(param_1 + 0x9e);
  return *(undefined4 *)(param_1 + 0xa6);
}

/* 0x80016de0 FUN_80016de0 */
undefined4 FUN_80016de0(char *param_1,char *param_2,undefined4 *param_3)

{
  undefined4 uVar1;
  
  *param_2 = *param_1;
  *param_3 = *(undefined4 *)(param_1 + 2);
  param_3[1] = *(undefined4 *)(param_1 + 10);
  if (*param_2 == '\0') {
    uVar1 = 1;
  }
  else if (param_1[0x21] == '\0') {
    *(undefined1 *)(param_3 + 2) = 0x2e;
    *(undefined1 *)((int)param_3 + 9) = 0;
    uVar1 = 0;
  }
  else if (param_1[0x21] == '\x01') {
    *(undefined1 *)(param_3 + 2) = 0x2e;
    *(undefined1 *)((int)param_3 + 9) = 0x2e;
    *(undefined1 *)((int)param_3 + 10) = 0;
    uVar1 = 0;
  }
  else {
    FUN_80019cdc(param_3 + 2,param_1 + 0x21,param_1[0x20]);
    *(undefined1 *)((int)param_3 + (byte)param_1[0x20] + 8) = 0;
    uVar1 = 0;
  }
  return uVar1;
}

/* 0x80017044 FUN_80017044 */
undefined4 FUN_80017044(undefined4 param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined1 auStack_810 [8];
  undefined1 auStack_808 [2040];
  undefined4 local_10 [2];
  
  FUN_8001657c(0x10,auStack_810,1,0x800,1,1);
  iVar1 = FUN_8001997c(auStack_808,"PLAYSTATION",0xb);
  if (iVar1 == 0) {
    FUN_80016ccc(auStack_810,local_10);
    FUN_8001657c(local_10[0],param_1,1,0x800,1,1);
    uVar2 = 0;
  }
  else {
    uVar2 = 0xffffffff;
  }
  return uVar2;
}

/* 0x80017114 FUN_80017114 */
undefined4 FUN_80017114(undefined4 param_1)

{
  int iVar1;
  int local_118;
  undefined4 local_114 [2];
  undefined1 auStack_10c [256];
  byte local_c [4];
  
  local_118 = 0;
  while ((local_118 < 0x800 &&
         (iVar1 = FUN_80016de0(&DAT_8002c2c8 + local_118,local_c,local_114), iVar1 == 0))) {
    local_118 = local_118 + (uint)local_c[0];
    iVar1 = FUN_800198cc(auStack_10c,param_1);
    if (iVar1 == 0) {
      return local_114[0];
    }
  }
  return 0;
}

/* 0x800171f0 FUN_800171f0 */
uint FUN_800171f0(void)

{
  setCopReg(0,Status,Status | 1);
  return Status | 1;
}

/* 0x80017240 FUN_80017240 */
void FUN_80017240(void)

{
  DAT_80045041 = 1;
  return;
}

/* 0x8001726c FUN_8001726c */
void FUN_8001726c(void)

{
  byte bVar1;
  uint uVar2;
  
  bVar1 = DAT_bf801803 & 7;
  DAT_bf801800 = 1;
  DAT_bf801803 = 0x40;
  FUN_80016c24(3);
  DAT_80022dc5 = 0;
  while ((DAT_bf801800 & 0x20) != 0) {
    uVar2 = (uint)DAT_80022dc5;
    DAT_80022dc5 = DAT_80022dc5 + 1;
    (&DAT_8002c2b8)[uVar2] = DAT_bf801801;
  }
  switch(bVar1) {
  case 1:
    FUN_800169ec();
    break;
  case 2:
    FUN_80016ac0();
    break;
  case 3:
    FUN_80016af8();
    break;
  case 4:
    FUN_80016b34();
    break;
  case 5:
    FUN_80016b60();
  }
  return;
}

/* 0x8001742c FUN_8001742c */
void FUN_8001742c(void)

{
  int iVar1;
  
  iVar1 = FUN_80017a48(0);
  if (iVar1 != 0) {
    FUN_80017240();
  }
  iVar1 = FUN_80017a48(2);
  if (iVar1 != 0) {
    FUN_8001726c();
  }
  iVar1 = FUN_80017a48(9);
  if (iVar1 != 0) {
    FUN_80017e28(&DAT_8002cac8);
  }
  return;
}

/* 0x800174b8 FUN_800174b8 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_800174b8(void)

{
  FUN_8001763c();
  FUN_80017850(FUN_8001742c,0);
  _DAT_bf801074 = 0x205;
  FUN_800171f0();
  return;
}

/* 0x80017518 FUN_80017518 */
void FUN_80017518(void)

{
  do {
  } while (DAT_80045041 != '\x01');
  DAT_80045041 = 0;
  return;
}

/* 0x80017560 FUN_80017560 */
bool FUN_80017560(void)

{
  setCopReg(0,Status,Status & 0xfffffffe);
  return (Status & 1) != 0;
}

/* 0x800175cc FUN_800175cc */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_800175cc(void)

{
  setCopReg(0,Status,0x50000000);
  _DAT_bf801074 = 0;
  _DAT_bf801070 = 0;
  _DAT_bf8010f0 = 0;
  _DAT_bf8010f4 = 0x7f000000;
  return;
}

/* 0x8001763c FUN_8001763c */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_8001763c(void)

{
  _DAT_bf801074 = 0;
  _DAT_bf801070 = 0;
  _DAT_bf8010f0 = 0;
  _DAT_bf8010f4 = 0x7f000000;
  setCopReg(0,Status,0x10000000);
  _DAT_80045044 = _DAT_80000310;
  DAT_8002c218 = _DAT_80000040;
  DAT_8002c21c = _DAT_80000044;
  DAT_8002c220 = _DAT_80000048;
  DAT_8002c224 = _DAT_8000004c;
  DAT_8002c228 = _DAT_80000080;
  DAT_8002c22c = _DAT_80000084;
  DAT_8002c230 = _DAT_80000088;
  DAT_8002c234 = _DAT_8000008c;
  _DAT_80000040 = DAT_8001be30;
  _DAT_80000044 = DAT_8001be34;
  _DAT_80000048 = DAT_8001be38;
  _DAT_8000004c = DAT_8001be3c;
  _DAT_80000080 = DAT_8001be30;
  _DAT_80000084 = DAT_8001be34;
  _DAT_80000088 = DAT_8001be38;
  _DAT_8000008c = DAT_8001be3c;
  (*_DAT_80000310)();
  _DAT_bf8010f0 = 0xbbbbbbb;
  _DAT_bf8010f4 = 0x800000;
  setCopReg(0,Status,0x50000401);
  return 0x50000401;
}

/* 0x80017850 FUN_80017850 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80017850(undefined4 param_1,undefined4 param_2)

{
  FUN_80017560();
  _DAT_80045048 = param_1;
  _DAT_8004504c = param_2;
  return;
}

/* 0x800178a4 FUN_800178a4 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

bool FUN_800178a4(void)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  
  FUN_80017560();
  (*(code *)&SUB_bfc00000)();
  FUN_800175cc();
  iVar2 = FUN_80019684(0xbfc00108,"Sony Computer Entertainment Inc.",0x20);
  if (iVar2 != 0) {
    (*(code *)&SUB_bfc00000)();
  }
  _DAT_80000040 = DAT_8001bfcc;
  _DAT_80000044 = DAT_8001bfd0;
  _DAT_80000048 = DAT_8001bfd4;
  _DAT_8000004c = DAT_8001bfd8;
  DAT_80030000 = DAT_8001bfdc;
  DAT_80030004 = DAT_8001bfe0;
  DAT_80030008 = DAT_8001bfe4;
  DAT_8003000c = DAT_8001bfe8;
  uVar3 = DAT_8001bfe4;
  FUN_80019e44();
  setCopReg(0,HWREna,0);
  setCopReg(0,PageMask,0x80030000);
  setCopReg(0,Count,0xffff0000);
  setCopReg(0,HWREna,0xea800000);
  (*(code *)&SUB_bfc00390)();
  bVar1 = ((int)(uint)_DAT_bf801070 >> (uVar3 & 0x1f) & 1U) != 0;
  if (bVar1) {
    _DAT_bf801070 = ~(ushort)(1 << (uVar3 & 0x1f));
  }
  return bVar1;
}

/* 0x800178c8 FUN_800178c8 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

bool FUN_800178c8(void)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  
  FUN_800175cc();
  iVar2 = FUN_80019684(0xbfc00108,"Sony Computer Entertainment Inc.",0x20);
  if (iVar2 != 0) {
    (*(code *)&SUB_bfc00000)();
  }
  _DAT_80000040 = DAT_8001bfcc;
  _DAT_80000044 = DAT_8001bfd0;
  _DAT_80000048 = DAT_8001bfd4;
  _DAT_8000004c = DAT_8001bfd8;
  DAT_80030000 = DAT_8001bfdc;
  DAT_80030004 = DAT_8001bfe0;
  DAT_80030008 = DAT_8001bfe4;
  DAT_8003000c = DAT_8001bfe8;
  uVar3 = DAT_8001bfe4;
  FUN_80019e44();
  setCopReg(0,HWREna,0);
  setCopReg(0,PageMask,0x80030000);
  setCopReg(0,Count,0xffff0000);
  setCopReg(0,HWREna,0xea800000);
  (*(code *)&SUB_bfc00390)();
  bVar1 = ((int)(uint)_DAT_bf801070 >> (uVar3 & 0x1f) & 1U) != 0;
  if (bVar1) {
    _DAT_bf801070 = ~(ushort)(1 << (uVar3 & 0x1f));
  }
  return bVar1;
}

/* 0x80017a48 FUN_80017a48 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

bool FUN_80017a48(uint param_1)

{
  bool bVar1;
  
  bVar1 = ((int)(uint)_DAT_bf801070 >> (param_1 & 0x1f) & 1U) != 0;
  if (bVar1) {
    _DAT_bf801070 = ~(ushort)(1 << (param_1 & 0x1f));
  }
  return bVar1;
}

/* 0x80017ad4 FUN_80017ad4 */
undefined4 FUN_80017ad4(int param_1,int param_2)

{
  int local_res4;
  
  local_res4 = param_2;
  while( true ) {
    if (local_res4 < 1) {
      return 0;
    }
    if ((*(uint *)(param_1 * 0x10 + -0x407fef78) & 0x1000000) == 0) break;
    FUN_80016c24(10);
    local_res4 = local_res4 + -10;
  }
  return 1;
}

/* 0x80017b74 FUN_80017b74 */
int FUN_80017b74(int param_1)

{
  return (uint)*(ushort *)(param_1 + 0x10) * (uint)*(ushort *)(param_1 + 0x16);
}

/* 0x80017bc4 FUN_80017bc4 */
int FUN_80017bc4(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar2 = *(int *)(param_1 + 0xc);
  iVar1 = FUN_80017b74(param_1);
  return iVar2 + iVar1 * param_2;
}

/* 0x80017c2c FUN_80017c2c */
bool FUN_80017c2c(int *param_1)

{
  return *param_1 != 0;
}

/* 0x80017c68 FUN_80017c68 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80017c68(uint *param_1)

{
  ushort uVar1;
  uint local_18;
  uint local_14;
  int local_10;
  
  uVar1 = _DAT_bf801daa;
  if ((short)param_1[2] == 0) {
    _DAT_bf801daa = _DAT_bf801daa & 0xffbf;
  }
  else if ((_DAT_bf801daa & 0x40) == 0) {
    local_18 = *param_1;
    local_14 = FUN_80017bc4(param_1,(short)param_1[1]);
    _DAT_bf801da4 = (undefined2)(local_14 >> 3);
    _DAT_bf801daa = uVar1 | 0x40;
    local_10 = 0;
    for (; local_18 != 0; local_18 = local_18 >> 1) {
      if ((local_18 & 1) != 0) {
        *(short *)(local_10 * 0x10 + -0x407fe3f2) = (short)(local_14 >> 3);
        local_14 = local_14 + (ushort)param_1[4];
      }
      local_10 = local_10 + 1;
    }
  }
  return;
}

/* 0x80017e28 FUN_80017e28 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80017e28(int param_1)

{
  char cVar1;
  
  cVar1 = FUN_80017c2c(param_1);
  if (cVar1 == '\x01') {
    _DAT_bf801daa = _DAT_bf801daa & 0xffbf;
    *(short *)(param_1 + 4) =
         (short)((*(ushort *)(param_1 + 4) + 1) % (uint)*(ushort *)(param_1 + 0x12));
    *(short *)(param_1 + 8) = *(short *)(param_1 + 8) + -1;
    FUN_80017c68(param_1);
  }
  return;
}

/* 0x80017f14 FUN_80017f14 */
undefined4 FUN_80017f14(undefined1 param_1,undefined1 param_2,undefined1 param_3,undefined1 param_4)

{
  return CONCAT13(param_4,CONCAT12(param_3,CONCAT11(param_2,param_1)));
}

/* 0x80017f90 FUN_80017f90 */
undefined4 FUN_80017f90(undefined2 param_1,undefined2 param_2)

{
  return CONCAT22(param_2,param_1);
}

/* 0x80017fd8 FUN_80017fd8 */
uint FUN_80017fd8(uint param_1)

{
  return param_1 >> 0x18 | param_1 >> 8 & 0xff00 | (param_1 & 0xff00) << 8 | param_1 << 0x18;
}

/* 0x80018044 FUN_80018044 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80018044(undefined2 param_1,undefined2 param_2)

{
  _DAT_bf801d80 = param_1;
  _DAT_bf801d82 = param_1;
  _DAT_bf801d84 = param_2;
  _DAT_bf801d86 = param_2;
  return;
}

/* 0x800180cc FUN_800180cc */
void FUN_800180cc(uint param_1)

{
  FUN_80018548(1 << (param_1 & 0x1f));
  return;
}

/* 0x80018118 FUN_80018118 */
undefined4 FUN_80018118(int *param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  
  iVar3 = *param_1;
  iVar1 = FUN_80017f14(0x56,0x41,0x47,0x70);
  if ((iVar3 == iVar1) && (*(ushort *)((int)param_1 + 0x1e) < 2)) {
    uVar2 = 1;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}

/* 0x800181ac FUN_800181ac */
uint FUN_800181ac(int param_1)

{
  int iVar1;
  
  iVar1 = FUN_80017fd8(*(undefined4 *)(param_1 + 0x10));
  return (uint)(iVar1 << 0xc) / 0xac44 & 0xffff;
}

/* 0x8001820c FUN_8001820c */
void FUN_8001820c(int param_1)

{
  FUN_80017fd8(*(undefined4 *)(param_1 + 0xc));
  return;
}

/* 0x80018254 FUN_80018254 */
int FUN_80018254(int param_1)

{
  return param_1 + 0x30;
}

/* 0x80018284 FUN_80018284 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_80018284(ushort param_1,ushort param_2)

{
  int local_10;
  
  local_10 = 10000;
  while( true ) {
    if (local_10 < 1) {
      return 0;
    }
    if (param_2 == (_DAT_bf801dae & param_1)) break;
    FUN_80016c24(10);
    local_10 = local_10 + -10;
  }
  return 1;
}

/* 0x80018340 FUN_80018340 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80018340(void)

{
  undefined4 local_10;
  
  _DAT_bf801014 = 0x200931e1;
  _DAT_bf801daa = 0;
  FUN_80018284(0x3f,0);
  _DAT_bf801d80 = 0;
  _DAT_bf801d82 = 0;
  _DAT_bf801d84 = 0;
  _DAT_bf801d86 = 0;
  _DAT_bf801da2 = 0xfffe;
  _DAT_bf801d90 = 0;
  _DAT_bf801d92 = 0;
  _DAT_bf801d94 = 0;
  _DAT_bf801d96 = 0;
  _DAT_bf801d98 = 0;
  _DAT_bf801d9a = 0;
  _DAT_bf801daa = 0x8000;
  FUN_80018284(0x3f,0);
  _DAT_bf801dac = 4;
  _DAT_bf801da6 = 0x200;
  _DAT_bf801da8 = 0x500;
  for (local_10 = 7; 0 < local_10; local_10 = local_10 + -1) {
    _DAT_bf801da8 = 0;
  }
  _DAT_bf801daa = 0x8010;
  FUN_80018284(0x430,0x10);
  FUN_80016c24(100);
  _DAT_bf801daa = 0xc000;
  FUN_800180cc(0xffffff);
  _DAT_bf8010f0 = _DAT_bf8010f0 | 0x80000;
  FUN_80018044(0x3fff,0);
  return;
}

/* 0x80018548 FUN_80018548 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80018548(uint param_1)

{
  undefined4 local_res0;
  undefined4 local_10;
  
  local_res0 = param_1 & 0xffffff;
  _DAT_bf801d8c = (undefined2)local_res0;
  _DAT_bf801d8e = (undefined2)(local_res0 >> 0x10);
  local_10 = 0;
  for (; local_res0 != 0; local_res0 = local_res0 >> 1) {
    if ((local_res0 & 1) != 0) {
      *(undefined2 *)(local_10 * 0x10 + -0x407fe400) = 0;
      *(undefined2 *)(local_10 * 0x10 + -0x407fe3fe) = 0;
      *(undefined2 *)(local_10 * 0x10 + -0x407fe3fc) = 0x1000;
      *(undefined2 *)(local_10 * 0x10 + -0x407fe3fa) = 0x200;
    }
    local_10 = local_10 + 1;
  }
  _DAT_bf801d88 = 0;
  _DAT_bf801d8a = 0;
  return;
}

/* 0x800186e4 FUN_800186e4 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int FUN_800186e4(uint param_1,undefined4 param_2,uint param_3,char param_4)

{
  char cVar1;
  ushort uVar2;
  int iVar3;
  uint uVar4;
  
  uVar4 = ((param_3 >> 2) + 3) / 4;
  cVar1 = FUN_80017ad4(4,100000);
  if (cVar1 == '\x01') {
    uVar2 = _DAT_bf801daa & 0xffcf;
    _DAT_bf801daa = uVar2;
    FUN_80018284(0x30,0);
    _DAT_bf801dac = 4;
    _DAT_bf801da6 = (undefined2)(param_1 >> 3);
    _DAT_bf801daa = uVar2 | 0x20;
    FUN_80018284(0x30,0x20);
    _DAT_bf8010c0 = param_2;
    _DAT_bf8010c4 = FUN_80017f90(4,uVar4 & 0xffff);
    _DAT_bf8010c8 = 0x1000201;
    if (param_4 != '\0') {
      FUN_80017ad4(4,100000);
    }
    iVar3 = uVar4 << 4;
  }
  else {
    iVar3 = 0;
  }
  return iVar3;
}

/* 0x800188d0 FUN_800188d0 */
void FUN_800188d0(undefined4 *param_1)

{
  *param_1 = 0;
  *(undefined2 *)(param_1 + 1) = 0;
  *(undefined2 *)((int)param_1 + 6) = 0;
  return;
}

/* 0x8001891c FUN_8001891c */
bool FUN_8001891c(undefined4 *param_1,undefined4 param_2,undefined4 param_3)

{
  char cVar1;
  undefined2 uVar2;
  
  cVar1 = FUN_80018118(param_2);
  if (cVar1 == '\x01') {
    *param_1 = param_3;
    uVar2 = FUN_800181ac(param_2);
    *(undefined2 *)(param_1 + 1) = uVar2;
    uVar2 = FUN_8001820c(param_2);
    *(undefined2 *)((int)param_1 + 6) = uVar2;
  }
  return cVar1 == '\x01';
}

/* 0x800189c4 FUN_800189c4 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint FUN_800189c4(uint *param_1,undefined2 param_2,undefined2 param_3,uint param_4)

{
  if (((int)param_4 < 0) || (0x17 < (int)param_4)) {
    param_4 = 0xffffffff;
  }
  else if (*param_1 == 0) {
    param_4 = 0xfffffffe;
  }
  else {
    *(undefined2 *)(param_4 * 0x10 + -0x407fe400) = param_2;
    *(undefined2 *)(param_4 * 0x10 + -0x407fe3fe) = param_3;
    *(short *)(param_4 * 0x10 + -0x407fe3fc) = (short)param_1[1];
    *(short *)(param_4 * 0x10 + -0x407fe3fa) = (short)(*param_1 >> 3);
    *(undefined2 *)(param_4 * 0x10 + -0x407fe3f8) = 0xff;
    *(undefined2 *)(param_4 * 0x10 + -0x407fe3f6) = 0;
    if ((int)param_4 < 0x10) {
      _DAT_bf801d88 = (undefined2)(1 << (param_4 & 0x1f));
    }
    else {
      _DAT_bf801d8a = (undefined2)(1 << (param_4 - 0x10 & 0x1f));
    }
  }
  return param_4;
}

/* 0x80018bdc FUN_80018bdc */
undefined4 FUN_80018bdc(int param_1,int param_2)

{
  char cVar1;
  undefined4 uVar2;
  int iVar3;
  
  *(uint *)(param_1 + 0x10) = (uint)(*(int *)(param_1 + 0x10) << 1) / 3;
  FUN_800188d0(param_2);
  cVar1 = FUN_8001891c(param_2,param_1,DAT_80022d64);
  iVar3 = DAT_80022d64;
  if (cVar1 == '\x01') {
    uVar2 = FUN_80018254(param_1);
    iVar3 = FUN_800186e4(iVar3,uVar2,*(undefined2 *)(param_2 + 6),1);
    DAT_80022d64 = DAT_80022d64 + iVar3;
    uVar2 = 0;
  }
  else {
    uVar2 = 2;
  }
  return uVar2;
}

/* 0x80018d00 FUN_80018d00 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint FUN_80018d00(int param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = param_1 + _DAT_80045018 + 7 & 0xfffffff8;
  uVar2 = _DAT_80045018;
  if (_DAT_8004501c <= uVar1) {
    uVar2 = 0;
    uVar1 = _DAT_80045018;
  }
  _DAT_80045018 = uVar1;
  return uVar2;
}

/* 0x80018d84 entry */
undefined4 processEntry entry(void)

{
  undefined4 uVar1;
  undefined4 in_a0;
  undefined4 in_a1;
  undefined4 uStack00000000;
  undefined4 uStack00000004;
  undefined4 *local_18;
  undefined4 *local_14;
  undefined4 *local_10;
  
  uStack00000000 = in_a0;
  uStack00000004 = in_a1;
  FUN_80019c10(&DAT_80022d70,0,0x11d70);
  for (local_18 = (undefined4 *)&DAT_80022d4c; local_18 < &DAT_80022d4c; local_18 = local_18 + 1) {
    (*(code *)*local_18)();
  }
  for (local_14 = (undefined4 *)&DAT_80022d4c; local_14 < &DAT_80022d4c; local_14 = local_14 + 1) {
    (*(code *)*local_14)();
  }
  uVar1 = FUN_80011a28(uStack00000000,uStack00000004);
  for (local_10 = (undefined4 *)&DAT_80022d4c; local_10 < &DAT_80022d4c; local_10 = local_10 + 1) {
    (*(code *)*local_10)();
  }
  return uVar1;
}

/* 0x80018f04 FUN_80018f04 */
int FUN_80018f04(int param_1,uint param_2)

{
  int local_10;
  
  for (local_10 = param_1;
      (local_10 != 0 &&
      ((*(int *)(local_10 + 4) == 0 ||
       ((uint)(*(int *)(local_10 + 4) - (*(int *)(local_10 + 8) + *(int *)(local_10 + 0xc))) <
        param_2)))); local_10 = *(int *)(local_10 + 4)) {
  }
  return local_10;
}

/* 0x80018ff0 FUN_80018ff0 */
int * FUN_80018ff0(int param_1)

{
  int *piVar1;
  uint uVar2;
  undefined4 *puVar3;
  int iVar4;
  int *piVar5;
  
  puVar3 = DAT_80022da0;
  if (param_1 == 0) {
    piVar1 = (int *)0x0;
  }
  else {
    uVar2 = param_1 + 0x17U & 0xfffffff8;
    if (DAT_80022da4 == (undefined4 *)0x0) {
      if (DAT_80022da0 == (undefined4 *)0x0) {
        DAT_80022da0 = (undefined4 *)FUN_80018d00(0);
      }
      puVar3 = (undefined4 *)FUN_80018d00(uVar2);
      if (puVar3 == (undefined4 *)0x0) {
        piVar1 = (int *)0x0;
      }
      else {
        piVar1 = puVar3 + 4;
        puVar3[2] = piVar1;
        puVar3[3] = uVar2 - 0x10;
        *puVar3 = 0;
        puVar3[1] = 0;
        DAT_80022da4 = puVar3;
        DAT_80022da8 = puVar3;
      }
    }
    else if ((undefined4 *)((int)DAT_80022da0 + uVar2) < DAT_80022da4) {
      piVar1 = DAT_80022da0 + 4;
      DAT_80022da0[2] = piVar1;
      puVar3[3] = uVar2 - 0x10;
      *puVar3 = 0;
      puVar3[1] = DAT_80022da4;
      *DAT_80022da4 = puVar3;
      DAT_80022da4 = puVar3;
    }
    else {
      iVar4 = FUN_80018f04(DAT_80022da4,uVar2);
      if (iVar4 == 0) {
        puVar3 = (undefined4 *)FUN_80018d00(uVar2);
        if (puVar3 == (undefined4 *)0x0) {
          piVar1 = (int *)0x0;
        }
        else {
          piVar1 = puVar3 + 4;
          puVar3[2] = piVar1;
          puVar3[3] = uVar2 - 0x10;
          *puVar3 = DAT_80022da8;
          puVar3[1] = 0;
          DAT_80022da8[1] = puVar3;
          DAT_80022da8 = puVar3;
        }
      }
      else {
        piVar5 = (int *)(*(int *)(iVar4 + 8) + *(int *)(iVar4 + 0xc));
        piVar1 = piVar5 + 4;
        piVar5[2] = (int)piVar1;
        piVar5[3] = uVar2 - 0x10;
        *piVar5 = iVar4;
        piVar5[1] = *(int *)(iVar4 + 4);
        *(int **)piVar5[1] = piVar5;
        *(int **)(iVar4 + 4) = piVar5;
      }
    }
  }
  return piVar1;
}

/* 0x8001936c FUN_8001936c */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8001936c(undefined1 param_1)

{
  do {
  } while ((_DAT_bf801054 & 0x101) == 0x100);
  if ((_DAT_bf801054 & 0x100) != 0) {
    DAT_bf801050 = param_1;
  }
  return;
}

/* 0x800193f8 FUN_800193f8 */
int FUN_800193f8(char *param_1)

{
  char *local_res0;
  int local_10;
  
  local_10 = 1;
  for (local_res0 = param_1; *local_res0 != '\0'; local_res0 = local_res0 + 1) {
    FUN_8001936c((int)*local_res0);
    local_10 = local_10 + 1;
  }
  FUN_8001936c(10);
  return local_10;
}

/* 0x80019498 FUN_80019498 */
void FUN_80019498(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  FUN_8001bce0("%s:%d: assert(%s)\n",param_1,param_2,param_3);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

/* 0x800194d8 FUN_800194d8 */
undefined4 FUN_800194d8(int param_1)

{
  undefined4 uVar1;
  
  if ((param_1 == 0x20) || ((8 < param_1 && (param_1 < 0xe)))) {
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

/* 0x80019544 FUN_80019544 */
undefined4 FUN_80019544(int param_1)

{
  undefined4 uVar1;
  
  if (((param_1 < 0x41) || (0x5a < param_1)) && ((param_1 < 0x61 || (0x7a < param_1)))) {
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}

/* 0x800195c8 FUN_800195c8 */
undefined4 FUN_800195c8(int param_1)

{
  undefined4 uVar1;
  
  if ((param_1 < 0x30) || (0x39 < param_1)) {
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}

/* 0x80019624 FUN_80019624 */
int FUN_80019624(int param_1)

{
  undefined4 local_res0;
  
  local_res0 = param_1;
  if ((0x60 < param_1) && (param_1 < 0x7b)) {
    local_res0 = param_1 + -0x20;
  }
  return local_res0;
}

/* 0x80019684 FUN_80019684 */
int FUN_80019684(byte *param_1,byte *param_2,int param_3)

{
  int local_res8;
  byte *local_18;
  byte *local_14;
  
  local_res8 = param_3;
  local_18 = param_1;
  local_14 = param_2;
  while( true ) {
    if (local_res8 == 0) {
      return 0;
    }
    if (*local_18 != *local_14) break;
    local_res8 = local_res8 + -1;
    local_18 = local_18 + 1;
    local_14 = local_14 + 1;
  }
  return (uint)*local_18 - (uint)*local_14;
}

/* 0x80019758 FUN_80019758 */
char * FUN_80019758(char *param_1,char *param_2)

{
  char *local_res4;
  char *local_10;
  
  local_res4 = param_2;
  local_10 = param_1;
  while (*local_res4 != '\0') {
    *local_10 = *local_res4;
    local_res4 = local_res4 + 1;
    local_10 = local_10 + 1;
  }
  *local_10 = '\0';
  return param_1;
}

/* 0x800197e8 FUN_800197e8 */
char * FUN_800197e8(char *param_1,char *param_2,int param_3)

{
  char *local_res4;
  int local_res8;
  char *local_10;
  
  local_res4 = param_2;
  local_res8 = param_3;
  local_10 = param_1;
  while ((local_res8 != 0 && (*local_res4 != '\0'))) {
    *local_10 = *local_res4;
    local_res8 = local_res8 + -1;
    local_res4 = local_res4 + 1;
    local_10 = local_10 + 1;
  }
  for (; local_res8 != 0; local_res8 = local_res8 + -1) {
    *local_10 = '\0';
    local_10 = local_10 + 1;
  }
  return param_1;
}

/* 0x800198cc FUN_800198cc */
int FUN_800198cc(char *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  char *pcVar3;
  char *pcVar4;
  char *local_res0;
  char *local_res4;
  
  local_res0 = param_1;
  local_res4 = param_2;
  do {
    pcVar3 = local_res0 + 1;
    cVar1 = *local_res0;
    pcVar4 = local_res4 + 1;
    cVar2 = *local_res4;
    if (cVar1 != cVar2) {
      return (int)cVar1 - (int)cVar2;
    }
    local_res0 = pcVar3;
    local_res4 = pcVar4;
  } while ((cVar1 != '\0') || (cVar2 != '\0'));
  return 0;
}

/* 0x8001997c FUN_8001997c */
int FUN_8001997c(char *param_1,char *param_2,int param_3)

{
  char *local_res0;
  char *local_res4;
  int local_res8;
  
  local_res0 = param_1;
  local_res4 = param_2;
  local_res8 = param_3;
  while (((local_res8 != 0 && (*local_res0 != '\0')) && (*local_res4 != '\0'))) {
    if (*local_res0 != *local_res4) {
      return (int)*local_res0 - (int)*local_res4;
    }
    local_res8 = local_res8 + -1;
    local_res0 = local_res0 + 1;
    local_res4 = local_res4 + 1;
  }
  return 0;
}

/* 0x80019a68 FUN_80019a68 */
char * FUN_80019a68(char *param_1,int param_2)

{
  char *local_res0;
  
  local_res0 = param_1;
  while( true ) {
    if (*local_res0 == '\0') {
      return (char *)0x0;
    }
    if (param_2 == *local_res0) break;
    local_res0 = local_res0 + 1;
  }
  return local_res0;
}

/* 0x80019af4 FUN_80019af4 */
char * FUN_80019af4(char *param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  char *local_res0;
  
  iVar1 = FUN_80019ba4(param_2);
  local_res0 = param_1;
  if (iVar1 != 0) {
    for (; *local_res0 != '\0'; local_res0 = local_res0 + 1) {
      iVar2 = FUN_80019684(local_res0,param_2,iVar1);
      if (iVar2 == 0) {
        return local_res0;
      }
    }
    param_1 = (char *)0x0;
  }
  return param_1;
}

/* 0x80019ba4 FUN_80019ba4 */
int FUN_80019ba4(char *param_1)

{
  char *local_res0;
  int local_10;
  
  local_10 = 0;
  for (local_res0 = param_1; *local_res0 != '\0'; local_res0 = local_res0 + 1) {
    local_10 = local_10 + 1;
  }
  return local_10;
}

/* 0x80019c10 FUN_80019c10 */
/* WARNING: Instruction at (ram,0x80019c8a) overlaps instruction at (ram,0x80019c88)
    */
/* WARNING: Control flow encountered bad instruction data */

undefined1 * FUN_80019c10(undefined1 *param_1,undefined1 param_2,int param_3)

{
  undefined1 *puVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  
  puVar1 = param_1;
  if (param_3 + -0x20 < 1) {
    for (; 0 < param_3; param_3 = param_3 + -1) {
      *puVar1 = param_2;
      puVar1 = puVar1 + 1;
    }
    return param_1;
  }
  uVar5 = CONCAT22(CONCAT11(param_2,param_2),CONCAT11(param_2,param_2));
  uVar3 = (uint)param_1 & 3;
  *(uint *)(param_1 + -uVar3) =
       *(uint *)(param_1 + -uVar3) & 0xffffffffU >> (4 - uVar3) * 8 | uVar5 << uVar3 * 8;
  iVar2 = ((uint)param_1 & 3) - 4;
  puVar4 = (uint *)(param_1 + -iVar2);
  uVar6 = param_3 + iVar2;
  uVar3 = uVar6 & 3;
  iVar2 = uVar6 - uVar3;
  while (iVar7 = iVar2 + -0x40, iVar7 < 0) {
    puVar4 = (uint *)((int)puVar4 + iVar7);
    switch(iVar2) {
    default:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 5:
    case 6:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 9:
    case 10:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xd:
    case 0xe:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x11:
    case 0x12:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x15:
    case 0x16:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x19:
    case 0x1a:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x1d:
    case 0x1e:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x21:
    case 0x22:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x25:
    case 0x26:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x29:
    case 0x2a:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x2d:
    case 0x2e:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x31:
    case 0x32:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x35:
    case 0x36:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x39:
    case 0x3a:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x3b:
    case 0x3c:
switchD_80019c80_caseD_3b:
      puVar4[1] = uVar5;
    case 0x37:
    case 0x38:
      puVar4[2] = uVar5;
    case 0x33:
    case 0x34:
      puVar4[3] = uVar5;
    case 0x2f:
    case 0x30:
      puVar4[4] = uVar5;
    case 0x2b:
    case 0x2c:
      puVar4[5] = uVar5;
    case 0x27:
    case 0x28:
      puVar4[6] = uVar5;
    case 0x23:
    case 0x24:
      puVar4[7] = uVar5;
    case 0x1f:
    case 0x20:
      puVar4[8] = uVar5;
    case 0x1b:
    case 0x1c:
      puVar4[9] = uVar5;
    case 0x17:
    case 0x18:
      puVar4[10] = uVar5;
    case 0x13:
    case 0x14:
      puVar4[0xb] = uVar5;
    case 0xf:
    case 0x10:
      puVar4[0xc] = uVar5;
    case 0xb:
    case 0xc:
      puVar4[0xd] = uVar5;
    case 7:
    case 8:
      puVar4[0xe] = uVar5;
    case 3:
    case 4:
      puVar4[0xf] = uVar5;
    case 0:
      puVar4 = puVar4 + 0x10;
      iVar2 = iVar7;
      if (iVar7 < 1) {
        puVar1 = (undefined1 *)((int)puVar4 + (uVar3 - 1));
        uVar3 = (uint)puVar1 & 3;
        puVar4 = (uint *)(puVar1 + -uVar3);
        *puVar4 = *puVar4 & -1 << (uVar3 + 1) * 8 | uVar5 >> (3 - uVar3) * 8;
        return param_1;
      }
      break;
    case 0x3d:
    case 0x3e:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x3f:
      goto switchD_80019c80_caseD_3f;
    }
  }
switchD_80019c80_caseD_3f:
  *puVar4 = uVar5;
  goto switchD_80019c80_caseD_3b;
}

/* 0x80019cdc FUN_80019cdc */
/* WARNING: Instruction at (ram,0x80019d72) overlaps instruction at (ram,0x80019d70)
    */
/* WARNING: Control flow encountered bad instruction data */

undefined4 * FUN_80019cdc(undefined4 *param_1,undefined4 *param_2,int param_3)

{
  bool bVar1;
  int iVar2;
  undefined4 *puVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  code *UNRECOVERED_JUMPTABLE;
  
  puVar3 = param_1;
  if (param_3 + -0x20 < 1) {
    for (; 0 < param_3; param_3 = param_3 + -1) {
      *(undefined1 *)puVar3 = *(undefined1 *)param_2;
      param_2 = (undefined4 *)((int)param_2 + 1);
      puVar3 = (undefined4 *)((int)puVar3 + 1);
    }
    return param_1;
  }
  *param_1 = *param_2;
  iVar2 = ((uint)param_2 & 3) - 4;
  iVar5 = (int)param_2 - iVar2;
  uVar4 = (int)param_1 - iVar2;
  iVar2 = (param_3 + iVar2) - (param_3 + iVar2 & 3U);
  UNRECOVERED_JUMPTABLE = (code *)&UNK_80019e00;
  bVar1 = (uVar4 & 3) != 0;
  if (bVar1) {
    UNRECOVERED_JUMPTABLE = (code *)&LAB_80019db0;
  }
  iVar6 = iVar2 + -0x20;
  if (iVar6 < 0) {
    iVar5 = iVar5 + iVar6;
    switch(iVar2) {
    default:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 5:
    case 6:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 9:
    case 10:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0xd:
    case 0xe:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x11:
    case 0x12:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x15:
    case 0x16:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x19:
    case 0x1a:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x1b:
    case 0x1c:
switchD_80019d68_caseD_1b:
    case 0x17:
    case 0x18:
    case 0x13:
    case 0x14:
    case 0xf:
    case 0x10:
    case 0xb:
    case 0xc:
    case 7:
    case 8:
    case 3:
    case 4:
    case 0:
      if (iVar6 < 0) {
                    /* WARNING: Could not recover jumptable at 0x80019da0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        puVar3 = (undefined4 *)(*(UNRECOVERED_JUMPTABLE + -(iVar6 << (uint)bVar1)))(uVar4 + iVar6);
        return puVar3;
      }
                    /* WARNING: Could not recover jumptable at 0x80019da8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      puVar3 = (undefined4 *)(*UNRECOVERED_JUMPTABLE)(uVar4,iVar5 + 0x20);
      return puVar3;
    case 0x1d:
    case 0x1e:
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    case 0x1f:
      break;
    }
  }
  goto switchD_80019d68_caseD_1b;
}

/* 0x80019e44 FUN_80019e44 */
void FUN_80019e44(void)

{
  (*(code *)&LAB_a0019e5c)(0xfffe0000);
  return;
}

/* 0x80019ee4 FUN_80019ee4 */
void FUN_80019ee4(undefined1 param_1,int param_2,uint param_3,uint param_4)

{
  if (param_3 < param_4) {
    *(undefined1 *)(param_2 + param_3) = param_1;
  }
  return;
}

/* 0x80019f50 FUN_80019f50 */
void FUN_80019f50(void)

{
  return;
}

/* 0x80019f88 FUN_80019f88 */
void FUN_80019f88(char param_1)

{
  if (param_1 != '\0') {
    FUN_8001936c((int)param_1);
  }
  return;
}

/* 0x80019fec FUN_80019fec */
int FUN_80019fec(char *param_1,int param_2)

{
  int local_res4;
  char *local_10;
  
  local_res4 = param_2;
  local_10 = param_1;
  while ((*local_10 != '\0' && (local_res4 != 0))) {
    local_10 = local_10 + 1;
    local_res4 = local_res4 + -1;
  }
  return (int)local_10 - (int)param_1;
}

/* 0x8001a078 FUN_8001a078 */
undefined4 FUN_8001a078(char param_1)

{
  undefined4 uVar1;
  
  if ((param_1 < '0') || ('9' < param_1)) {
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}

/* 0x8001a0e0 FUN_8001a0e0 */
int FUN_8001a0e0(undefined4 *param_1)

{
  char *pcVar1;
  int iVar2;
  int local_10;
  
  local_10 = 0;
  while (iVar2 = FUN_8001a078((int)*(char *)*param_1), iVar2 != 0) {
    pcVar1 = (char *)*param_1;
    *param_1 = pcVar1 + 1;
    local_10 = local_10 * 10 + (int)*pcVar1 + -0x30;
  }
  return local_10;
}

/* 0x8001a198 FUN_8001a198 */
int FUN_8001a198(code *param_1,undefined4 param_2,int param_3,undefined4 param_4,int param_5,
                uint param_6,uint param_7,uint param_8)

{
  int local_res8;
  uint local_10;
  
  local_res8 = param_3;
  if (((param_8 & 2) == 0) && (local_res8 = param_3, (param_8 & 1) == 0)) {
    local_res8 = param_3;
    for (local_10 = param_6; local_10 < param_7; local_10 = local_10 + 1) {
      (*param_1)(0x20,param_2,local_res8,param_4);
      local_res8 = local_res8 + 1;
    }
  }
  while (param_6 != 0) {
    param_6 = param_6 - 1;
    (*param_1)((int)*(char *)(param_5 + param_6),param_2,local_res8,param_4);
    local_res8 = local_res8 + 1;
  }
  if ((param_8 & 2) != 0) {
    while ((uint)(local_res8 - param_3) < param_7) {
      (*param_1)(0x20,param_2,local_res8,param_4);
      local_res8 = local_res8 + 1;
    }
  }
  return local_res8;
}

/* 0x8001a340 FUN_8001a340 */
void FUN_8001a340(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 int param_5,uint param_6,char param_7,int param_8,uint param_9,uint param_10,
                 uint param_11)

{
  uint uVar1;
  
  if ((param_11 & 2) == 0) {
    if (((param_10 != 0) && ((param_11 & 1) != 0)) && ((param_7 != '\0' || ((param_11 & 0xc) != 0)))
       ) {
      param_10 = param_10 - 1;
    }
    while ((param_6 < param_9 && (param_6 < 0x20))) {
      *(undefined1 *)(param_5 + param_6) = 0x30;
      param_6 = param_6 + 1;
    }
    while ((((param_11 & 1) != 0 && (param_6 < param_10)) && (param_6 < 0x20))) {
      *(undefined1 *)(param_5 + param_6) = 0x30;
      param_6 = param_6 + 1;
    }
  }
  if ((param_11 & 0x10) != 0) {
    uVar1 = param_6;
    if (((((param_11 & 0x400) == 0) && (param_6 != 0)) &&
        ((param_6 == param_9 || (param_6 == param_10)))) &&
       ((uVar1 = param_6 - 1, uVar1 != 0 && (param_8 == 0x10)))) {
      uVar1 = param_6 - 2;
    }
    param_6 = uVar1;
    if (((param_8 == 0x10) && ((param_11 & 0x20) == 0)) && (param_6 < 0x20)) {
      *(undefined1 *)(param_5 + param_6) = 0x78;
      param_6 = param_6 + 1;
    }
    else if (((param_8 == 0x10) && ((param_11 & 0x20) != 0)) && (param_6 < 0x20)) {
      *(undefined1 *)(param_5 + param_6) = 0x58;
      param_6 = param_6 + 1;
    }
    else if ((param_8 == 2) && (param_6 < 0x20)) {
      *(undefined1 *)(param_5 + param_6) = 0x62;
      param_6 = param_6 + 1;
    }
    if (param_6 < 0x20) {
      *(undefined1 *)(param_5 + param_6) = 0x30;
      param_6 = param_6 + 1;
    }
  }
  if (param_6 < 0x20) {
    if (param_7 == '\0') {
      if ((param_11 & 4) == 0) {
        if ((param_11 & 8) != 0) {
          *(undefined1 *)(param_5 + param_6) = 0x20;
          param_6 = param_6 + 1;
        }
      }
      else {
        *(undefined1 *)(param_5 + param_6) = 0x2b;
        param_6 = param_6 + 1;
      }
    }
    else {
      *(undefined1 *)(param_5 + param_6) = 0x2d;
      param_6 = param_6 + 1;
    }
  }
  FUN_8001a198(param_1,param_2,param_3,param_4,param_5,param_6,param_10,param_11);
  return;
}

/* 0x8001a7a0 FUN_8001a7a0 */
void FUN_8001a7a0(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 uint param_5,undefined1 param_6,uint param_7,undefined4 param_8,undefined4 param_9,
                 uint param_10)

{
  int iVar1;
  uint uVar2;
  uint local_30;
  undefined1 auStack_28 [32];
  
  local_30 = 0;
  if (param_5 == 0) {
    param_10 = param_10 & 0xffffffef;
  }
  if (((param_10 & 0x400) == 0) || (param_5 != 0)) {
    do {
      uVar2 = param_5 % param_7;
      if ((char)uVar2 < '\n') {
        iVar1 = (uVar2 & 0xff) + 0x30;
      }
      else {
        if ((param_10 & 0x20) == 0) {
          iVar1 = 0x61;
        }
        else {
          iVar1 = 0x41;
        }
        iVar1 = ((int)((iVar1 + (uVar2 & 0xff)) * 0x1000000) >> 0x18) + -10;
      }
      uVar2 = local_30 + 1;
      auStack_28[local_30] = (char)((uint)(iVar1 * 0x1000000) >> 0x18);
      param_5 = param_5 / param_7;
      local_30 = uVar2;
    } while ((param_5 != 0) && (uVar2 < 0x20));
  }
  FUN_8001a340(param_1,param_2,param_3,param_4,auStack_28,local_30,param_6,param_7,param_8,param_9,
               param_10);
  return;
}

/* 0x8001a970 FUN_8001a970 */
void FUN_8001a970(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 int param_5,int param_6,undefined1 param_7,undefined4 param_8,undefined4 param_9,
                 undefined4 param_10,undefined4 param_11,undefined4 param_12,uint param_13)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  longlong lVar4;
  uint local_30;
  undefined1 auStack_28 [32];
  
  local_30 = 0;
  if (param_5 == 0 && param_6 == 0) {
    param_13 = param_13 & 0xffffffef;
  }
  lVar4 = CONCAT44(param_6,param_5);
  if (((param_13 & 0x400) == 0) || (lVar4 = CONCAT44(param_6,param_5), param_5 != 0 || param_6 != 0)
     ) {
    do {
      param_6 = (int)((ulonglong)lVar4 >> 0x20);
      param_5 = (int)lVar4;
      bVar1 = FUN_8001c7c8(param_5,param_6,param_9,param_10);
      if ((char)bVar1 < '\n') {
        iVar2 = bVar1 + 0x30;
      }
      else {
        if ((param_13 & 0x20) == 0) {
          iVar2 = 0x61;
        }
        else {
          iVar2 = 0x41;
        }
        iVar2 = ((int)((iVar2 + (uint)bVar1) * 0x1000000) >> 0x18) + -10;
      }
      uVar3 = local_30 + 1;
      auStack_28[local_30] = (char)((uint)(iVar2 * 0x1000000) >> 0x18);
      lVar4 = FUN_8001c358(param_5,param_6,param_9,param_10);
      local_30 = uVar3;
    } while ((lVar4 != 0) && (uVar3 < 0x20));
  }
  FUN_8001a340(param_1,param_2,param_3,param_4,auStack_28,local_30,param_7,param_9,param_11,param_12
               ,param_13);
  return;
}

/* 0x8001ab64 FUN_8001ab64 */
/* WARNING: Removing unreachable block (ram,0x8001bab4) */

uint FUN_8001ab64(code *param_1,int param_2,uint param_3,char *param_4,uint *param_5)

{
  bool bVar1;
  char *pcVar2;
  int iVar3;
  int *piVar4;
  uint uVar5;
  undefined4 *puVar6;
  uint uVar7;
  code *local_res0;
  char *local_resc;
  uint local_68;
  uint local_64;
  uint local_60;
  uint local_58;
  undefined4 local_54;
  uint local_50;
  char *local_4c;
  uint local_48;
  
  local_58 = 0;
  local_res0 = param_1;
  local_resc = param_4;
  if (param_2 == 0) {
    local_res0 = FUN_80019f50;
  }
  while (*local_resc != '\0') {
    if (*local_resc == '%') {
      local_resc = local_resc + 1;
      local_68 = 0;
      do {
        switch(*local_resc) {
        case ' ':
          local_68 = local_68 | 8;
          local_resc = local_resc + 1;
          bVar1 = true;
          break;
        default:
          bVar1 = false;
          break;
        case '#':
          local_68 = local_68 | 0x10;
          local_resc = local_resc + 1;
          bVar1 = true;
          break;
        case '+':
          local_68 = local_68 | 4;
          local_resc = local_resc + 1;
          bVar1 = true;
          break;
        case '-':
          local_68 = local_68 | 2;
          local_resc = local_resc + 1;
          bVar1 = true;
          break;
        case '0':
          local_68 = local_68 | 1;
          local_resc = local_resc + 1;
          bVar1 = true;
        }
      } while (bVar1);
      local_64 = 0;
      iVar3 = FUN_8001a078((int)*local_resc);
      if (iVar3 == 0) {
        if (*local_resc == '*') {
          local_64 = *param_5;
          if ((int)local_64 < 0) {
            local_68 = local_68 | 2;
            local_64 = -local_64;
          }
          local_resc = local_resc + 1;
          param_5 = param_5 + 1;
        }
      }
      else {
        local_64 = FUN_8001a0e0(&local_resc);
      }
      local_60 = 0;
      if (*local_resc == '.') {
        local_68 = local_68 | 0x400;
        local_resc = local_resc + 1;
        iVar3 = FUN_8001a078((int)*local_resc);
        if (iVar3 == 0) {
          if (*local_resc == '*') {
            local_60 = *param_5;
            if ((int)local_60 < 0) {
              local_60 = 0;
            }
            local_resc = local_resc + 1;
            param_5 = param_5 + 1;
          }
        }
        else {
          local_60 = FUN_8001a0e0(&local_resc);
        }
      }
      switch(*local_resc) {
      case 'h':
        pcVar2 = local_resc + 1;
        uVar5 = local_68 | 0x80;
        if (local_resc[1] == 'h') {
          pcVar2 = local_resc + 2;
          uVar5 = local_68 | 0xc0;
        }
        break;
      default:
        pcVar2 = local_resc;
        uVar5 = local_68;
        break;
      case 'j':
        pcVar2 = local_resc + 1;
        uVar5 = local_68 | 0x200;
        break;
      case 'l':
        pcVar2 = local_resc + 1;
        uVar5 = local_68 | 0x100;
        if (local_resc[1] == 'l') {
          pcVar2 = local_resc + 2;
          uVar5 = local_68 | 0x300;
        }
        break;
      case 't':
        pcVar2 = local_resc + 1;
        uVar5 = local_68 | 0x100;
        break;
      case 'z':
        pcVar2 = local_resc + 1;
        uVar5 = local_68 | 0x100;
      }
      local_68 = uVar5;
      local_resc = pcVar2;
      switch(*local_resc) {
      case '%':
        (*local_res0)(0x25,param_2,local_58,param_3);
        local_resc = local_resc + 1;
        local_58 = local_58 + 1;
        break;
      default:
        (*local_res0)((int)*local_resc,param_2,local_58,param_3);
        local_resc = local_resc + 1;
        local_58 = local_58 + 1;
        break;
      case 'X':
      case 'b':
      case 'd':
      case 'i':
      case 'o':
      case 'u':
      case 'x':
        if ((*local_resc == 'x') || (*local_resc == 'X')) {
          local_54 = 0x10;
        }
        else if (*local_resc == 'o') {
          local_54 = 8;
        }
        else if (*local_resc == 'b') {
          local_54 = 2;
        }
        else {
          local_54 = 10;
          local_68 = local_68 & 0xffffffef;
        }
        if (*local_resc == 'X') {
          local_68 = local_68 | 0x20;
        }
        if ((*local_resc != 'i') && (*local_resc != 'd')) {
          local_68 = local_68 & 0xfffffff3;
        }
        if ((local_68 & 0x400) != 0) {
          local_68 = local_68 & 0xfffffffe;
        }
        if ((*local_resc == 'i') || (*local_resc == 'd')) {
          if ((local_68 & 0x200) == 0) {
            if ((local_68 & 0x100) == 0) {
              if ((local_68 & 0x40) == 0) {
                if ((local_68 & 0x80) == 0) {
                  uVar5 = *param_5;
                }
                else {
                  uVar5 = (uint)(short)*param_5;
                }
              }
              else {
                uVar5 = (uint)(char)*param_5;
              }
              param_5 = param_5 + 1;
              uVar7 = uVar5;
              if ((int)uVar5 < 0) {
                uVar7 = -uVar5;
              }
              local_58 = FUN_8001a7a0(local_res0,param_2,local_58,param_3,uVar7,uVar5 >> 0x1f,
                                      local_54,local_60,local_64,local_68);
            }
            else {
              uVar7 = *param_5;
              uVar5 = uVar7;
              if ((int)uVar7 < 0) {
                uVar5 = -uVar7;
              }
              local_58 = FUN_8001a7a0(local_res0,param_2,local_58,param_3,uVar5,uVar7 >> 0x1f,
                                      local_54,local_60,local_64,local_68);
              param_5 = param_5 + 1;
            }
          }
          else {
            piVar4 = (int *)((int)param_5 + 7U & 0xfffffff8);
            uVar7 = piVar4[1];
            iVar3 = *piVar4;
            uVar5 = uVar7;
            if ((int)uVar7 < 0) {
              iVar3 = -iVar3;
              uVar5 = -(uint)(iVar3 != 0) - uVar7;
            }
            local_58 = FUN_8001a970(local_res0,param_2,local_58,param_3,iVar3,uVar5,uVar7 >> 0x1f);
            param_5 = (uint *)(piVar4 + 2);
          }
        }
        else if ((local_68 & 0x200) == 0) {
          if ((local_68 & 0x100) == 0) {
            if ((local_68 & 0x40) == 0) {
              if ((local_68 & 0x80) == 0) {
                uVar5 = *param_5;
              }
              else {
                uVar5 = *param_5 & 0xffff;
              }
            }
            else {
              uVar5 = *param_5 & 0xff;
            }
            param_5 = param_5 + 1;
            local_58 = FUN_8001a7a0(local_res0,param_2,local_58,param_3,uVar5,0,local_54,local_60,
                                    local_64,local_68);
          }
          else {
            local_58 = FUN_8001a7a0(local_res0,param_2,local_58,param_3,*param_5,0,local_54,local_60
                                    ,local_64,local_68);
            param_5 = param_5 + 1;
          }
        }
        else {
          puVar6 = (undefined4 *)((int)param_5 + 7U & 0xfffffff8);
          local_58 = FUN_8001a970(local_res0,param_2,local_58,param_3,*puVar6,puVar6[1],0);
          param_5 = puVar6 + 2;
        }
        local_resc = local_resc + 1;
        break;
      case 'c':
        local_50 = 1;
        if ((local_68 & 2) == 0) {
          while (uVar5 = local_50 + 1, bVar1 = local_50 < local_64, local_50 = uVar5, bVar1) {
            (*local_res0)(0x20,param_2,local_58,param_3);
            local_58 = local_58 + 1;
          }
        }
        uVar5 = local_58 + 1;
        (*local_res0)((int)(char)*param_5,param_2,local_58,param_3);
        local_58 = uVar5;
        if ((local_68 & 2) != 0) {
          while (local_50 < local_64) {
            (*local_res0)(0x20,param_2,local_58,param_3);
            local_58 = local_58 + 1;
            local_50 = local_50 + 1;
          }
        }
        local_resc = local_resc + 1;
        param_5 = param_5 + 1;
        break;
      case 'p':
        local_58 = FUN_8001a7a0(local_res0,param_2,local_58,param_3,*param_5,0,0x10,local_60,8,
                                local_68 | 0x21);
        param_5 = param_5 + 1;
        local_resc = local_resc + 1;
        break;
      case 's':
        local_4c = (char *)*param_5;
        uVar5 = local_60;
        if (local_60 == 0) {
          uVar5 = 0xffffffff;
        }
        uVar5 = FUN_80019fec(local_4c,uVar5);
        local_48 = uVar5;
        if (((local_68 & 0x400) != 0) && (local_48 = local_60, uVar5 < local_60)) {
          local_48 = uVar5;
        }
        if ((local_68 & 2) == 0) {
          while (uVar5 = local_48 + 1, bVar1 = local_48 < local_64, local_48 = uVar5, bVar1) {
            (*local_res0)(0x20,param_2,local_58,param_3);
            local_58 = local_58 + 1;
          }
        }
        while ((*local_4c != '\0' &&
               (((local_68 & 0x400) == 0 || (bVar1 = local_60 != 0, local_60 = local_60 - 1, bVar1))
               ))) {
          (*local_res0)((int)*local_4c,param_2,local_58,param_3);
          local_58 = local_58 + 1;
          local_4c = local_4c + 1;
        }
        if ((local_68 & 2) != 0) {
          while (local_48 < local_64) {
            (*local_res0)(0x20,param_2,local_58,param_3);
            local_58 = local_58 + 1;
            local_48 = local_48 + 1;
          }
        }
        local_resc = local_resc + 1;
        param_5 = param_5 + 1;
      }
    }
    else {
      (*local_res0)((int)*local_resc,param_2,local_58,param_3);
      local_resc = local_resc + 1;
      local_58 = local_58 + 1;
    }
  }
  uVar5 = local_58;
  if (param_3 <= local_58) {
    uVar5 = param_3 - 1;
  }
  (*local_res0)(0,param_2,uVar5,param_3);
  return local_58;
}

/* 0x8001bce0 FUN_8001bce0 */
undefined4 FUN_8001bce0(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  undefined4 local_res4;
  undefined4 local_res8;
  undefined4 local_resc;
  undefined1 auStack_10 [8];
  
  local_res4 = param_2;
  local_res8 = param_3;
  local_resc = param_4;
  uVar1 = FUN_8001ab64(FUN_80019f88,auStack_10,0xffffffff,param_1,&local_res4);
  return uVar1;
}

/* 0x8001bd50 FUN_8001bd50 */
undefined4 FUN_8001bd50(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  undefined4 local_res8;
  undefined4 local_resc;
  
  local_res8 = param_3;
  local_resc = param_4;
  uVar1 = FUN_8001ab64(FUN_80019ee4,param_1,0xffffffff,param_2,&local_res8);
  return uVar1;
}

/* 0x8001bdc0 FUN_8001bdc0 */
undefined4 FUN_8001bdc0(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  undefined4 local_resc;
  
  local_resc = param_4;
  uVar1 = FUN_8001ab64(FUN_80019ee4,param_1,param_2,param_3,&local_resc);
  return uVar1;
}

/* 0x8001bfec FUN_8001bfec */
void FUN_8001bfec(uint param_1)

{
  uint local_res0;
  int local_10;
  
  local_res0 = param_1;
  for (local_10 = 8; local_10 != 0; local_10 = local_10 + -1) {
    FUN_8001936c((int)"0123456789abcdef"[local_res0 >> 0x1c]);
    local_res0 = local_res0 << 4;
  }
  return;
}

/* 0x8001c08c FUN_8001c08c */
void FUN_8001c08c(int param_1,undefined4 param_2)

{
  int iVar1;
  char *pcVar2;
  char *local_20;
  undefined4 *local_1c;
  uint local_18;
  undefined4 *local_14;
  
  FUN_800193f8((&PTR_s_Load_address_error_80022cd0)[param_1 + -4]);
  if (param_1 < 6) {
    FUN_8001936c(0x40);
    FUN_8001936c(0x20);
    FUN_8001bfec(param_2);
    FUN_8001936c(10);
  }
  FUN_800193f8("Register dump:");
  local_20 = "pcatv0v1a0a1a2a3t0t1t2t3t4t5t6t7s0s1s2s3s4s5s6s7t8t9gpspfprahilo";
  local_1c = (undefined4 *)PTR_DAT_80022d5c;
  for (local_18 = 0x1f; -1 < (int)local_18; local_18 = local_18 - 1) {
    FUN_8001936c(0x20);
    FUN_8001936c(0x20);
    pcVar2 = local_20 + 1;
    FUN_8001936c((int)*local_20);
    local_20 = local_20 + 2;
    FUN_8001936c((int)*pcVar2);
    FUN_8001936c(0x3d);
    FUN_8001bfec(*local_1c);
    if ((local_18 & 3) == 0) {
      FUN_8001936c(10);
    }
    local_1c = local_1c + 1;
  }
  FUN_800193f8("Stack dump:");
  iVar1 = *(int *)(PTR_DAT_80022d5c + 0x6c);
  for (local_14 = (undefined4 *)(*(int *)(PTR_DAT_80022d5c + 0x6c) + -0x1c);
      local_14 <= (undefined4 *)(iVar1 + 0x1cU); local_14 = local_14 + 1) {
    if (*(undefined4 **)(PTR_DAT_80022d5c + 0x6c) == local_14) {
      FUN_8001936c(0x3e);
    }
    else {
      FUN_8001936c(0x20);
    }
    FUN_8001936c(0x20);
    FUN_8001bfec(local_14);
    FUN_8001936c(0x3a);
    FUN_8001936c(0x20);
    FUN_8001bfec(*local_14);
    FUN_8001936c(10);
  }
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

/* 0x8001c358 FUN_8001c358 */
ulonglong FUN_8001c358(uint param_1,uint param_2,uint param_3,uint param_4)

{
  longlong lVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  
  if (param_4 != 0) {
    uVar2 = LZCOUNT(param_4);
    if (param_2 < param_4) {
      uVar2 = 0;
    }
    else {
      if (uVar2 != 0) {
        uVar5 = 0x20 - uVar2;
        uVar3 = param_2 >> (uVar5 & 0x1f);
        uVar6 = param_3 >> (uVar5 & 0x1f) | param_4 << (uVar2 & 0x1f);
        uVar4 = uVar6 >> 0x10;
        if (uVar4 == 0) {
          trap(7);
        }
        uVar9 = uVar3 / uVar4;
        uVar7 = param_1 >> (uVar5 & 0x1f) | param_2 << (uVar2 & 0x1f);
        uVar5 = (uVar6 & 0xffff) * uVar9;
        uVar8 = uVar3 % uVar4 << 0x10 | uVar7 >> 0x10;
        uVar3 = uVar9;
        if (uVar8 < uVar5) {
          uVar8 = uVar8 + uVar6;
          uVar3 = uVar9 - 1;
          if ((uVar6 <= uVar8) && (uVar8 < uVar5)) {
            uVar3 = uVar9 - 2;
            uVar8 = uVar8 + uVar6;
          }
        }
        if (uVar4 == 0) {
          trap(7);
        }
        uVar10 = (uVar8 - uVar5) / uVar4;
        uVar9 = (uVar6 & 0xffff) * uVar10;
        uVar5 = (uVar8 - uVar5) % uVar4 << 0x10 | uVar7 & 0xffff;
        uVar4 = uVar10;
        if (uVar5 < uVar9) {
          uVar5 = uVar5 + uVar6;
          uVar4 = uVar10 - 1;
          if ((uVar6 <= uVar5) && (uVar5 < uVar9)) {
            uVar4 = uVar10 - 2;
            uVar5 = uVar5 + uVar6;
          }
        }
        uVar4 = uVar3 << 0x10 | uVar4;
        lVar1 = (ulonglong)uVar4 * (ulonglong)(param_3 << (uVar2 & 0x1f));
        uVar3 = (uint)((ulonglong)lVar1 >> 0x20);
        if ((uVar5 - uVar9 < uVar3) ||
           ((uVar5 - uVar9 == uVar3 && (param_1 << (uVar2 & 0x1f) < (uint)lVar1)))) {
          uVar4 = uVar4 - 1;
        }
        return (ulonglong)uVar4;
      }
      if (param_4 < param_2) {
        uVar2 = 1;
      }
      else {
        uVar2 = param_1 < param_3 ^ 1;
      }
    }
    return (ulonglong)uVar2;
  }
  uVar2 = LZCOUNT(param_3);
  if (param_2 < param_3) {
    if (uVar2 != 0) {
      uVar4 = param_1 >> (-uVar2 & 0x1f);
      param_3 = param_3 << (uVar2 & 0x1f);
      param_1 = param_1 << (uVar2 & 0x1f);
      param_2 = uVar4 | param_2 << (uVar2 & 0x1f);
    }
    uVar2 = param_3 >> 0x10;
    if (uVar2 == 0) {
      trap(7);
    }
    uVar6 = param_2 / uVar2;
    uVar4 = (param_3 & 0xffff) * uVar6;
    uVar3 = param_2 % uVar2 << 0x10 | param_1 >> 0x10;
    uVar5 = uVar6;
    if (uVar3 < uVar4) {
      uVar3 = uVar3 + param_3;
      uVar5 = uVar6 - 1;
      if ((param_3 <= uVar3) && (uVar3 < uVar4)) {
        uVar5 = uVar6 - 2;
        uVar3 = uVar3 + param_3;
      }
    }
    if (uVar2 == 0) {
      trap(7);
    }
    uVar7 = (uVar3 - uVar4) / uVar2;
    uVar6 = (param_3 & 0xffff) * uVar7;
    uVar4 = (uVar3 - uVar4) % uVar2 << 0x10 | param_1 & 0xffff;
    uVar3 = param_3 + uVar4;
    uVar2 = uVar7;
    if (((uVar4 < uVar6) && (uVar2 = uVar7 - 1, param_3 <= uVar3)) && (uVar3 < uVar6)) {
      uVar2 = uVar7 - 2;
    }
    return (ulonglong)(uVar5 << 0x10 | uVar2);
  }
  if (param_3 == 0) {
    trap(7);
    param_3 = 1 / 0;
  }
  uVar2 = LZCOUNT(param_3);
  if (uVar2 == 0) {
    param_2 = param_2 - param_3;
    uVar4 = param_3 >> 0x10;
    uVar2 = 1;
    uVar7 = param_3 & 0xffff;
  }
  else {
    param_3 = param_3 << (uVar2 & 0x1f);
    uVar4 = param_3 >> 0x10;
    uVar5 = param_2 >> (0x20 - uVar2 & 0x1f);
    if (uVar4 == 0) {
      trap(7);
    }
    uVar8 = uVar5 / uVar4;
    uVar7 = param_3 & 0xffff;
    uVar3 = param_1 >> (0x20 - uVar2 & 0x1f) | param_2 << (uVar2 & 0x1f);
    param_1 = param_1 << (uVar2 & 0x1f);
    uVar2 = uVar7 * uVar8;
    uVar6 = uVar5 % uVar4 << 0x10 | uVar3 >> 0x10;
    uVar5 = uVar8;
    if (uVar6 < uVar2) {
      uVar6 = uVar6 + param_3;
      uVar5 = uVar8 - 1;
      if ((param_3 <= uVar6) && (uVar6 < uVar2)) {
        uVar5 = uVar8 - 2;
        uVar6 = uVar6 + param_3;
      }
    }
    if (uVar4 == 0) {
      trap(7);
    }
    uVar9 = (uVar6 - uVar2) / uVar4;
    uVar8 = uVar7 * uVar9;
    param_2 = (uVar6 - uVar2) % uVar4 << 0x10 | uVar3 & 0xffff;
    uVar2 = uVar9;
    if (param_2 < uVar8) {
      param_2 = param_2 + param_3;
      uVar2 = uVar9 - 1;
      if ((param_3 <= param_2) && (param_2 < uVar8)) {
        uVar2 = uVar9 - 2;
        param_2 = param_2 + param_3;
      }
    }
    param_2 = param_2 - uVar8;
    uVar2 = uVar5 << 0x10 | uVar2;
  }
  if (uVar4 == 0) {
    trap(7);
  }
  uVar8 = param_2 / uVar4;
  uVar5 = uVar8 * uVar7;
  uVar6 = param_2 % uVar4 << 0x10 | param_1 >> 0x10;
  uVar3 = uVar8;
  if (uVar6 < uVar5) {
    uVar6 = uVar6 + param_3;
    uVar3 = uVar8 - 1;
    if ((param_3 <= uVar6) && (uVar6 < uVar5)) {
      uVar6 = uVar6 + param_3;
      uVar3 = uVar8 - 2;
    }
  }
  if (uVar4 == 0) {
    trap(7);
  }
  uVar8 = (uVar6 - uVar5) / uVar4;
  uVar4 = (uVar6 - uVar5) % uVar4 << 0x10 | param_1 & 0xffff;
  uVar5 = param_3 + uVar4;
  if (uVar4 < uVar8 * uVar7) {
    if ((uVar5 < param_3) || (uVar8 * uVar7 <= uVar5)) {
      uVar8 = uVar3 << 0x10 | uVar8 - 1;
      goto LAB_8001c568;
    }
    uVar8 = uVar8 - 2;
  }
  uVar8 = uVar3 << 0x10 | uVar8;
LAB_8001c568:
  return CONCAT44(uVar2,uVar8);
}

/* 0x8001c7c8 FUN_8001c7c8 */
undefined8 FUN_8001c7c8(uint param_1,uint param_2,uint param_3,uint param_4)

{
  longlong lVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  
  if (param_4 == 0) {
    uVar9 = LZCOUNT(param_3);
    if (param_2 < param_3) {
      if (uVar9 != 0) {
        uVar3 = param_1 >> (-uVar9 & 0x1f);
        param_3 = param_3 << (uVar9 & 0x1f);
        param_1 = param_1 << (uVar9 & 0x1f);
        param_2 = uVar3 | param_2 << (uVar9 & 0x1f);
      }
      uVar3 = param_3 >> 0x10;
      if (uVar3 == 0) {
        trap(7);
      }
      uVar2 = (param_2 / uVar3) * (param_3 & 0xffff);
      uVar6 = param_2 % uVar3 << 0x10 | param_1 >> 0x10;
      if (uVar6 < uVar2) {
        uVar6 = uVar6 + param_3;
        if ((param_3 <= uVar6) && (uVar6 < uVar2)) {
          uVar6 = uVar6 + param_3;
        }
      }
      if (uVar3 == 0) {
        trap(7);
      }
      uVar7 = ((uVar6 - uVar2) / uVar3) * (param_3 & 0xffff);
      uVar3 = (uVar6 - uVar2) % uVar3 << 0x10 | param_1 & 0xffff;
      if (((uVar3 < uVar7) && (uVar3 = uVar3 + param_3, param_3 <= uVar3)) && (uVar3 < uVar7)) {
        uVar3 = uVar3 + param_3;
      }
      param_2 = 0;
      param_1 = uVar3 - uVar7 >> (uVar9 & 0x1f);
    }
    else {
      if (param_3 == 0) {
        trap(7);
        param_3 = 1 / 0;
      }
      uVar9 = LZCOUNT(param_3);
      if (uVar9 == 0) {
        param_2 = param_2 - param_3;
        uVar2 = param_3 >> 0x10;
        uVar3 = param_3 & 0xffff;
      }
      else {
        param_3 = param_3 << (uVar9 & 0x1f);
        uVar2 = param_3 >> 0x10;
        uVar6 = param_2 >> (0x20 - uVar9 & 0x1f);
        uVar3 = param_3 & 0xffff;
        if (uVar2 == 0) {
          trap(7);
        }
        uVar4 = param_1 >> (0x20 - uVar9 & 0x1f);
        param_1 = param_1 << (uVar9 & 0x1f);
        uVar4 = uVar4 | param_2 << (uVar9 & 0x1f);
        uVar7 = (uVar6 / uVar2) * uVar3;
        uVar6 = uVar6 % uVar2 << 0x10 | uVar4 >> 0x10;
        if (uVar6 < uVar7) {
          uVar6 = uVar6 + param_3;
          if ((param_3 <= uVar6) && (uVar6 < uVar7)) {
            uVar6 = uVar6 + param_3;
          }
        }
        if (uVar2 == 0) {
          trap(7);
        }
        uVar5 = ((uVar6 - uVar7) / uVar2) * uVar3;
        param_2 = (uVar6 - uVar7) % uVar2 << 0x10 | uVar4 & 0xffff;
        if (param_2 < uVar5) {
          param_2 = param_2 + param_3;
          if ((param_3 <= param_2) && (param_2 < uVar5)) {
            param_2 = param_2 + param_3;
          }
        }
        param_2 = param_2 - uVar5;
      }
      if (uVar2 == 0) {
        trap(7);
      }
      uVar6 = (param_2 / uVar2) * uVar3;
      uVar7 = param_2 % uVar2 << 0x10 | param_1 >> 0x10;
      if (uVar7 < uVar6) {
        uVar7 = uVar7 + param_3;
        if ((param_3 <= uVar7) && (uVar7 < uVar6)) {
          uVar7 = uVar7 + param_3;
        }
      }
      if (uVar2 == 0) {
        trap(7);
      }
      uVar3 = ((uVar7 - uVar6) / uVar2) * uVar3;
      uVar2 = (uVar7 - uVar6) % uVar2 << 0x10 | param_1 & 0xffff;
      if (uVar2 < uVar3) {
        uVar2 = uVar2 + param_3;
        if ((param_3 <= uVar2) && (uVar2 < uVar3)) {
          uVar2 = uVar2 + param_3;
        }
      }
      param_2 = 0;
      param_1 = uVar2 - uVar3 >> (uVar9 & 0x1f);
    }
  }
  else if (param_4 <= param_2) {
    uVar9 = LZCOUNT(param_4);
    if (uVar9 == 0) {
      if ((param_4 < param_2) || (uVar9 = param_1, param_3 <= param_1)) {
        uVar9 = param_1 - param_3;
        param_2 = (param_2 - param_4) - (uint)(param_1 < uVar9);
      }
      return CONCAT44(param_2,uVar9);
    }
    uVar6 = 0x20 - uVar9;
    uVar2 = param_2 >> (uVar6 & 0x1f);
    uVar7 = param_3 >> (uVar6 & 0x1f) | param_4 << (uVar9 & 0x1f);
    uVar3 = uVar7 >> 0x10;
    if (uVar3 == 0) {
      trap(7);
    }
    uVar10 = uVar2 / uVar3;
    uVar5 = param_1 >> (uVar6 & 0x1f) | param_2 << (uVar9 & 0x1f);
    param_1 = param_1 << (uVar9 & 0x1f);
    uVar4 = (uVar7 & 0xffff) * uVar10;
    uVar8 = uVar2 % uVar3 << 0x10 | uVar5 >> 0x10;
    param_3 = param_3 << (uVar9 & 0x1f);
    uVar2 = uVar10;
    if (uVar8 < uVar4) {
      uVar8 = uVar8 + uVar7;
      uVar2 = uVar10 - 1;
      if ((uVar7 <= uVar8) && (uVar8 < uVar4)) {
        uVar2 = uVar10 - 2;
        uVar8 = uVar8 + uVar7;
      }
    }
    if (uVar3 == 0) {
      trap(7);
    }
    uVar11 = (uVar8 - uVar4) / uVar3;
    uVar10 = (uVar7 & 0xffff) * uVar11;
    uVar4 = (uVar8 - uVar4) % uVar3 << 0x10 | uVar5 & 0xffff;
    uVar3 = uVar11;
    if (uVar4 < uVar10) {
      uVar4 = uVar4 + uVar7;
      uVar3 = uVar11 - 1;
      if ((uVar7 <= uVar4) && (uVar4 < uVar10)) {
        uVar3 = uVar11 - 2;
        uVar4 = uVar4 + uVar7;
      }
    }
    uVar4 = uVar4 - uVar10;
    lVar1 = (ulonglong)(uVar2 << 0x10 | uVar3) * (ulonglong)param_3;
    uVar3 = (uint)lVar1;
    uVar2 = (uint)((ulonglong)lVar1 >> 0x20);
    if ((uVar4 < uVar2) || ((uVar4 == uVar2 && (param_1 < uVar3)))) {
      param_3 = uVar3 - param_3;
      lVar1 = CONCAT44((uVar2 - uVar7) - (uint)(uVar3 < param_3),param_3);
    }
    uVar3 = param_1 - (int)lVar1;
    uVar2 = (uVar4 - (int)((ulonglong)lVar1 >> 0x20)) - (uint)(param_1 < uVar3);
    return CONCAT44(uVar2 >> (uVar9 & 0x1f),uVar2 << (uVar6 & 0x1f) | uVar3 >> (uVar9 & 0x1f));
  }
  return CONCAT44(param_2,param_1);
}

