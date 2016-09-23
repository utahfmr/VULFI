#pragma once

// c++ system headers
#include <string>

// References used
// ---------
// Intel® Architecture Instruction Set Extensions Programming Reference
// https://software.intel.com/en-us/intel-architecture-instruction-set-extensions-programming-reference
// ---------
// Intel® 64 and IA-32 Architectures Software Developer Manuals
// http://www.intel.com/content/www/us/en/processors/architectures-software-developer-manuals.html
// ----------
// * http://www.felixcloutier.com/x86/


// Categorization only done for extensions: sse,sse2,sse3,ssse3,sse4a,sse41,sse42,avx,avx2,fma,bmi
// TODO - xop,mmx,3dnow extensions

static const string x86InstrinsicListMaskArg2[] = {
  "llvm.x86.avx2.maskload.d",
  "llvm.x86.avx2.maskload.d.256",
  "llvm.x86.avx2.maskload.q",
  "llvm.x86.avx2.maskload.q.256",
  "llvm.x86.avx.maskload.pd",
  "llvm.x86.avx.maskload.pd.256",
  "llvm.x86.avx.maskload.ps",
  "llvm.x86.avx.maskload.ps.256",
  "llvm.x86.sse2.maskmov.dqu",
  "llvm.x86.avx2.maskstore.d",
  "llvm.x86.avx2.maskstore.d.256",
  "llvm.x86.avx2.maskstore.q",
  "llvm.x86.avx2.maskstore.q.256",
  "llvm.x86.avx.maskstore.pd",
  "llvm.x86.avx.maskstore.pd.256",
  "llvm.x86.avx.maskstore.ps",
  "llvm.x86.avx.maskstore.ps.256",
  ""
};


static const string x86InstrinsicListBitWiseMaskArg2[] = {
  ""
};


static const string x86InstrinsicListCtrlMaskArg2[] = {
  "llvm.x86.ssse3.pshuf.b",
  "llvm.x86.ssse3.pshuf.b.128",
  "llvm.x86.sse.pshuf.w",
  ""
};

static const string x86InstrinsicListMaskArg3[] = {
  "llvm.x86.sse41.pblendvb",
  "llvm.x86.sse41.blendvpd",
  "llvm.x86.sse41.blendvps",
  ""
};

static const string x86InstrinsicListBitWiseMaskArg3[] = {
  "llvm.x86.sse41.dppd",
  "llvm.x86.sse41.dpps",
  "llvm.x86.avx.dp.ps.256",
  ""
};

static const string x86InstrinsicListCtrlMaskArg3[] = {
  "llvm.x86.avx2.pblendd.128",
  "llvm.x86.avx2.pblendd.256",
  "llvm.x86.avx2.pblendvb",
  "llvm.x86.avx2.pblendw",
  "llvm.x86.avx.blend.pd.256",
  "llvm.x86.avx.blend.ps.256",
  "llvm.x86.avx.blendv.pd.256",
  "llvm.x86.avx.blendv.ps.256",
  "llvm.x86.sse41.blendpd",
  "llvm.x86.sse41.blendps",
  "llvm.x86.sse41.pblendw",
  "llvm.x86.avx2.pshuf.b",
  ""
};

static const string x86InstrinsicListMaskArg4[] = {
  ""
};

static const string x86InstrinsicListBitWiseMaskArg4[] = {
  ""
};

static const string x86InstrinsicListBitWiseMaskArg5[] = {
  "llvm.x86.avx2.gather.d.d",
  "llvm.x86.avx2.gather.d.d.256",
  "llvm.x86.avx2.gather.d.pd",
  "llvm.x86.avx2.gather.d.pd.256",
  "llvm.x86.avx2.gather.d.ps",
  "llvm.x86.avx2.gather.d.ps.256",
  "llvm.x86.avx2.gather.d.q",
  "llvm.x86.avx2.gather.d.q.256",
  "llvm.x86.avx2.gather.q.d",
  "llvm.x86.avx2.gather.q.d.256",
  "llvm.x86.avx2.gather.q.pd",
  "llvm.x86.avx2.gather.q.pd.256",
  "llvm.x86.avx2.gather.q.ps",
  "llvm.x86.avx2.gather.q.ps.256",
  "llvm.x86.avx2.gather.q.q",
  "llvm.x86.avx2.gather.q.q.256",
  ""
};

static const string x86InstrinsicListCtrlMaskArg4[] = {
  ""
};

static const string x86InstrinsicListNoMask[] = {
  // 3dnow
  // Currently treating all AMD x86 intrinsics (3DNow,XOP) as nomask intrinsics
  // http://en.wikipedia.org/wiki/3DNow!
  "llvm.x86.3dnow.pavgusb",
  "llvm.x86.3dnow.pf2id",
  "llvm.x86.3dnow.pfacc",
  "llvm.x86.3dnow.pfadd",
  "llvm.x86.3dnow.pfcmpeq",
  "llvm.x86.3dnow.pfcmpge",
  "llvm.x86.3dnow.pfcmpgt",
  "llvm.x86.3dnow.pfmax",
  "llvm.x86.3dnow.pfmin",
  "llvm.x86.3dnow.pfmul",
  "llvm.x86.3dnow.pfrcp",
  "llvm.x86.3dnow.pfrcpit1",
  "llvm.x86.3dnow.pfrcpit2",
  "llvm.x86.3dnow.pfrsqit1",
  "llvm.x86.3dnow.pfrsqrt",
  "llvm.x86.3dnow.pfsub",
  "llvm.x86.3dnow.pfsubr",
  "llvm.x86.3dnow.pi2fd",
  "llvm.x86.3dnow.pmulhrw",
  "llvm.x86.3dnowa.pf2iw",
  "llvm.x86.3dnowa.pfnacc",
  "llvm.x86.3dnowa.pfpnacc",
  "llvm.x86.3dnowa.pi2fw",
  "llvm.x86.3dnowa.pswapd",
  // aesni
  "llvm.x86.aesni.aesdec",
  "llvm.x86.aesni.aesdeclast",
  "llvm.x86.aesni.aesenc",
  "llvm.x86.aesni.aesenclast",
  "llvm.x86.aesni.aesimc",
  "llvm.x86.aesni.aeskeygenassist",
  // avx2
  "llvm.x86.avx2.movntdqa", // nomask
  "llvm.x86.avx2.mpsadbw", // nomask
  "llvm.x86.avx2.pabs.b", // nomask
  "llvm.x86.avx2.pabs.d", // nomask
  "llvm.x86.avx2.pabs.w", // nomask
  "llvm.x86.avx2.packssdw", // nomask
  "llvm.x86.avx2.packsswb", // nomask
  "llvm.x86.avx2.packusdw", // nomask
  "llvm.x86.avx2.packuswb", // nomask
  "llvm.x86.avx2.padds.b", // nomask
  "llvm.x86.avx2.padds.w", // nomask
  "llvm.x86.avx2.paddus.b", // nomask
  "llvm.x86.avx2.paddus.w", // nomask
  "llvm.x86.avx2.pavg.b", // nomask
  "llvm.x86.avx2.pavg.w", // nomask
  "llvm.x86.avx2.pbroadcastb.128", // nomask
  "llvm.x86.avx2.pbroadcastb.256", // nomask
  "llvm.x86.avx2.pbroadcastd.128", // nomask
  "llvm.x86.avx2.pbroadcastd.256", // nomask
  "llvm.x86.avx2.pbroadcastq.128", // nomask
  "llvm.x86.avx2.pbroadcastq.256", // nomask
  "llvm.x86.avx2.pbroadcastw.128", // nomask
  "llvm.x86.avx2.pbroadcastw.256", // nomask
  "llvm.x86.avx2.permd", // nomask
  "llvm.x86.avx2.permps", // nomask
  "llvm.x86.avx2.phadd.d", // nomask
  "llvm.x86.avx2.phadd.sw", // nomask
  "llvm.x86.avx2.phadd.w", // nomask
  "llvm.x86.avx2.phsub.d", // nomask
  "llvm.x86.avx2.phsub.sw", // nomask
  "llvm.x86.avx2.phsub.w", // nomask
  "llvm.x86.avx2.pmadd.ub.sw", // nomask
  "llvm.x86.avx2.pmadd.wd", // nomask
  "llvm.x86.avx2.pmaxs.b", // nomask
  "llvm.x86.avx2.pmaxs.d", // nomask
  "llvm.x86.avx2.pmaxs.w", // nomask
  "llvm.x86.avx2.pmaxu.b", // nomask
  "llvm.x86.avx2.pmaxu.d", // nomask
  "llvm.x86.avx2.pmaxu.w", // nomask
  "llvm.x86.avx2.pmins.b", // nomask
  "llvm.x86.avx2.pmins.d", // nomask
  "llvm.x86.avx2.pmins.w", // nomask
  "llvm.x86.avx2.pminu.b", // nomask
  "llvm.x86.avx2.pminu.d", // nomask
  "llvm.x86.avx2.pminu.w", // nomask
  "llvm.x86.avx2.pmovmskb", // nomask - mask manipulation
  "llvm.x86.avx2.pmovsxbd", // nomask
  "llvm.x86.avx2.pmovsxbq", // nomask
  "llvm.x86.avx2.pmovsxbw", // nomask
  "llvm.x86.avx2.pmovsxdq", // nomask
  "llvm.x86.avx2.pmovsxwd", // nomask
  "llvm.x86.avx2.pmovsxwq", // nomask
  "llvm.x86.avx2.pmovzxbd", // nomask
  "llvm.x86.avx2.pmovzxbq", // nomask
  "llvm.x86.avx2.pmovzxbw", // nomask
  "llvm.x86.avx2.pmovzxdq", // nomask
  "llvm.x86.avx2.pmovzxwd", // nomask
  "llvm.x86.avx2.pmovzxwq", // nomask
  "llvm.x86.avx2.pmul.dq", // nomask
  "llvm.x86.avx2.pmul.hr.sw", // nomask
  "llvm.x86.avx2.pmulh.w", // nomask
  "llvm.x86.avx2.pmulhu.w", // nomask
  "llvm.x86.avx2.pmulu.dq", // nomask
  "llvm.x86.avx2.psad.bw", // nomask
  "llvm.x86.avx2.psign.b", // nomask
  "llvm.x86.avx2.psign.d", // nomask
  "llvm.x86.avx2.psign.w", // nomask
  "llvm.x86.avx2.psll.d", // nomask
  "llvm.x86.avx2.psll.dq", // nomask
  "llvm.x86.avx2.psll.dq.bs", // nomask
  "llvm.x86.avx2.psll.q", // nomask
  "llvm.x86.avx2.psll.w", // nomask
  "llvm.x86.avx2.pslli.d", // nomask
  "llvm.x86.avx2.pslli.q", // nomask
  "llvm.x86.avx2.pslli.w", // nomask
  "llvm.x86.avx2.psllv.d", // nomask
  "llvm.x86.avx2.psllv.d.256", // nomask
  "llvm.x86.avx2.psllv.q", // nomask
  "llvm.x86.avx2.psllv.q.256", // nomask
  "llvm.x86.avx2.psra.d", // nomask
  "llvm.x86.avx2.psra.w", // nomask
  "llvm.x86.avx2.psrai.d", // nomask
  "llvm.x86.avx2.psrai.w", // nomask
  "llvm.x86.avx2.psrav.d", // nomask
  "llvm.x86.avx2.psrav.d.256", // nomask
  "llvm.x86.avx2.psrl.d", // nomask
  "llvm.x86.avx2.psrl.dq", // nomask
  "llvm.x86.avx2.psrl.dq.bs", // nomask
  "llvm.x86.avx2.psrl.q", // nomask
  "llvm.x86.avx2.psrl.w", // nomask
  "llvm.x86.avx2.psrli.d", // nomask
  "llvm.x86.avx2.psrli.q", // nomask
  "llvm.x86.avx2.psrli.w", // nomask
  "llvm.x86.avx2.psrlv.d", // nomask
  "llvm.x86.avx2.psrlv.d.256", // nomask
  "llvm.x86.avx2.psrlv.q", // nomask
  "llvm.x86.avx2.psrlv.q.256", // nomask
  "llvm.x86.avx2.psubs.b", // nomask
  "llvm.x86.avx2.psubs.w", // nomask
  "llvm.x86.avx2.psubus.b", // nomask
  "llvm.x86.avx2.psubus.w", // nomask
  "llvm.x86.avx2.vbroadcast.sd.pd.256", // nomask
  "llvm.x86.avx2.vbroadcast.ss.ps", // nomask
  "llvm.x86.avx2.vbroadcast.ss.ps.256", // nomask
  "llvm.x86.avx2.vbroadcasti128", // nomask
  "llvm.x86.avx2.vextracti128", // nomask
  "llvm.x86.avx2.vinserti128", // nomask
  "llvm.x86.avx2.vperm2i128", // nomask
  // avx
  "llvm.x86.avx.addsub.pd.256", // nomask
  "llvm.x86.avx.addsub.ps.256", // nomask
  "llvm.x86.avx.cmp.pd.256", // nomask
  "llvm.x86.avx.cmp.ps.256", // nomask
  "llvm.x86.avx.cvt.pd2.ps.256", // nomask
  "llvm.x86.avx.cvt.pd2dq.256", // nomask
  "llvm.x86.avx.cvt.ps2.pd.256", // nomask
  "llvm.x86.avx.cvt.ps2dq.256", // nomask
  "llvm.x86.avx.cvtdq2.pd.256", // nomask
  "llvm.x86.avx.cvtdq2.ps.256", // nomask
  "llvm.x86.avx.cvtt.pd2dq.256", // nomask
  "llvm.x86.avx.cvtt.ps2dq.256", // nomask
  "llvm.x86.avx.hadd.pd.256", // nomask
  "llvm.x86.avx.hadd.ps.256", // nomask
  "llvm.x86.avx.hsub.pd.256", // nomask
  "llvm.x86.avx.hsub.ps.256", // nomask
  "llvm.x86.avx.ldu.dq.256", // nomask
  "llvm.x86.avx.max.pd.256", // nomask
  "llvm.x86.avx.max.ps.256", // nomask
  "llvm.x86.avx.min.pd.256", // nomask
  "llvm.x86.avx.min.ps.256", // nomask
  "llvm.x86.avx.movmsk.pd.256", // nomask - mask manipulation
  "llvm.x86.avx.movmsk.ps.256", // nomask - mask manipulation
  "llvm.x86.avx.ptestc.256", // nomask
  "llvm.x86.avx.ptestnzc.256", // nomask
  "llvm.x86.avx.ptestz.256", // nomask
  "llvm.x86.avx.rcp.ps.256", // nomask
  "llvm.x86.avx.round.pd.256", // nomask
  "llvm.x86.avx.round.ps.256", // nomask
  "llvm.x86.avx.rsqrt.ps.256", // nomask
  "llvm.x86.avx.sqrt.pd.256", // nomask
  "llvm.x86.avx.sqrt.ps.256", // nomask
  "llvm.x86.avx.storeu.dq.256", // nomask
  "llvm.x86.avx.storeu.pd.256", // nomask
  "llvm.x86.avx.storeu.ps.256", // nomask
  "llvm.x86.avx.vbroadcast.sd.256", // nomask
  "llvm.x86.avx.vbroadcast.ss", // nomask
  "llvm.x86.avx.vbroadcast.ss.256", // nomask
  "llvm.x86.avx.vbroadcastf128.pd.256", // nomask
  "llvm.x86.avx.vbroadcastf128.ps.256", // nomask
  "llvm.x86.avx.vextractf128.pd.256", // nomask
  "llvm.x86.avx.vextractf128.ps.256", // nomask
  "llvm.x86.avx.vextractf128.si.256", // nomask
  "llvm.x86.avx.vinsertf128.pd.256", // nomask
  "llvm.x86.avx.vinsertf128.ps.256", // nomask
  "llvm.x86.avx.vinsertf128.si.256", // nomask
  "llvm.x86.avx.vperm2f128.pd.256", // nomask
  "llvm.x86.avx.vperm2f128.ps.256", // nomask
  "llvm.x86.avx.vperm2f128.si.256", // nomask
  "llvm.x86.avx.vpermilvar.pd", // nomask
  "llvm.x86.avx.vpermilvar.pd.256", // nomask
  "llvm.x86.avx.vpermilvar.ps", // nomask
  "llvm.x86.avx.vpermilvar.ps.256", // nomask
  "llvm.x86.avx.vtestc.pd", // nomask
  "llvm.x86.avx.vtestc.pd.256", // nomask
  "llvm.x86.avx.vtestc.ps", // nomask
  "llvm.x86.avx.vtestc.ps.256", // nomask
  "llvm.x86.avx.vtestnzc.pd", // nomask
  "llvm.x86.avx.vtestnzc.pd.256", // nomask
  "llvm.x86.avx.vtestnzc.ps", // nomask
  "llvm.x86.avx.vtestnzc.ps.256", // nomask
  "llvm.x86.avx.vtestz.pd", // nomask
  "llvm.x86.avx.vtestz.pd.256", // nomask
  "llvm.x86.avx.vtestz.ps", // nomask
  "llvm.x86.avx.vtestz.ps.256", // nomask
  "llvm.x86.avx.vzeroall", // nomask
  "llvm.x86.avx.vzeroupper", // nomask
  // bmi
  "llvm.x86.bmi.bextr.32", // nomask
  "llvm.x86.bmi.bextr.64", // nomask
  "llvm.x86.bmi.bzhi.32", // nomask
  "llvm.x86.bmi.bzhi.64", // nomask
  "llvm.x86.bmi.pdep.32", // nomask
  "llvm.x86.bmi.pdep.64", // nomask
  "llvm.x86.bmi.pext.32", // nomask
  "llvm.x86.bmi.pext.64", // nomask
  // fma
  // http://en.wikipedia.org/wiki/FMA_instruction_set
  "llvm.x86.fma.vfmadd.pd", // nomask
  "llvm.x86.fma.vfmadd.pd.256", // nomask
  "llvm.x86.fma.vfmadd.ps", // nomask
  "llvm.x86.fma.vfmadd.ps.256", // nomask
  "llvm.x86.fma.vfmadd.sd", // nomask
  "llvm.x86.fma.vfmadd.ss", // nomask
  "llvm.x86.fma.vfmaddsub.pd", // nomask
  "llvm.x86.fma.vfmaddsub.pd.256", // nomask
  "llvm.x86.fma.vfmaddsub.ps", // nomask
  "llvm.x86.fma.vfmaddsub.ps.256", // nomask
  "llvm.x86.fma.vfmsub.pd", // nomask
  "llvm.x86.fma.vfmsub.pd.256", // nomask
  "llvm.x86.fma.vfmsub.ps", // nomask
  "llvm.x86.fma.vfmsub.ps.256", // nomask
  "llvm.x86.fma.vfmsub.sd", // nomask
  "llvm.x86.fma.vfmsub.ss", // nomask
  "llvm.x86.fma.vfmsubadd.pd", // nomask
  "llvm.x86.fma.vfmsubadd.pd.256", // nomask
  "llvm.x86.fma.vfmsubadd.ps", // nomask
  "llvm.x86.fma.vfmsubadd.ps.256", // nomask
  "llvm.x86.fma.vfnmadd.pd", // nomask
  "llvm.x86.fma.vfnmadd.pd.256", // nomask
  "llvm.x86.fma.vfnmadd.ps", // nomask
  "llvm.x86.fma.vfnmadd.ps.256", // nomask
  "llvm.x86.fma.vfnmadd.sd", // nomask
  "llvm.x86.fma.vfnmadd.ss", // nomask
  "llvm.x86.fma.vfnmsub.pd", // nomask
  "llvm.x86.fma.vfnmsub.pd.256", // nomask
  "llvm.x86.fma.vfnmsub.ps", // nomask
  "llvm.x86.fma.vfnmsub.ps.256", // nomask
  "llvm.x86.fma.vfnmsub.sd", // nomask
  "llvm.x86.fma.vfnmsub.ss", // nomask
  // miscellaneous
  "llvm.x86.int",
  // mmx
  "llvm.x86.mmx.emms",
  "llvm.x86.mmx.femms",
  "llvm.x86.mmx.maskmovq",
  "llvm.x86.mmx.movnt.dq",
  "llvm.x86.mmx.packssdw",
  "llvm.x86.mmx.packsswb",
  "llvm.x86.mmx.packuswb",
  "llvm.x86.mmx.padd.b",
  "llvm.x86.mmx.padd.d",
  "llvm.x86.mmx.padd.q",
  "llvm.x86.mmx.padd.w",
  "llvm.x86.mmx.padds.b",
  "llvm.x86.mmx.padds.w",
  "llvm.x86.mmx.paddus.b",
  "llvm.x86.mmx.paddus.w",
  "llvm.x86.mmx.palignr.b",
  "llvm.x86.mmx.pand",
  "llvm.x86.mmx.pandn",
  "llvm.x86.mmx.pavg.b",
  "llvm.x86.mmx.pavg.w",
  "llvm.x86.mmx.pcmpeq.b",
  "llvm.x86.mmx.pcmpeq.d",
  "llvm.x86.mmx.pcmpeq.w",
  "llvm.x86.mmx.pcmpgt.b",
  "llvm.x86.mmx.pcmpgt.d",
  "llvm.x86.mmx.pcmpgt.w",
  "llvm.x86.mmx.pextr.w",
  "llvm.x86.mmx.pinsr.w",
  "llvm.x86.mmx.pmadd.wd",
  "llvm.x86.mmx.pmaxs.w",
  "llvm.x86.mmx.pmaxu.b",
  "llvm.x86.mmx.pmins.w",
  "llvm.x86.mmx.pminu.b",
  "llvm.x86.mmx.pmovmskb",
  "llvm.x86.mmx.pmulh.w",
  "llvm.x86.mmx.pmulhu.w",
  "llvm.x86.mmx.pmull.w",
  "llvm.x86.mmx.pmulu.dq",
  "llvm.x86.mmx.por",
  "llvm.x86.mmx.psad.bw",
  "llvm.x86.mmx.psll.d",
  "llvm.x86.mmx.psll.q",
  "llvm.x86.mmx.psll.w",
  "llvm.x86.mmx.pslli.d",
  "llvm.x86.mmx.pslli.q",
  "llvm.x86.mmx.pslli.w",
  "llvm.x86.mmx.psra.d",
  "llvm.x86.mmx.psra.w",
  "llvm.x86.mmx.psrai.d",
  "llvm.x86.mmx.psrai.w",
  "llvm.x86.mmx.psrl.d",
  "llvm.x86.mmx.psrl.q",
  "llvm.x86.mmx.psrl.w",
  "llvm.x86.mmx.psrli.d",
  "llvm.x86.mmx.psrli.q",
  "llvm.x86.mmx.psrli.w",
  "llvm.x86.mmx.psub.b",
  "llvm.x86.mmx.psub.d",
  "llvm.x86.mmx.psub.q",
  "llvm.x86.mmx.psub.w",
  "llvm.x86.mmx.psubs.b",
  "llvm.x86.mmx.psubs.w",
  "llvm.x86.mmx.psubus.b",
  "llvm.x86.mmx.psubus.w",
  "llvm.x86.mmx.punpckhbw",
  "llvm.x86.mmx.punpckhdq",
  "llvm.x86.mmx.punpckhwd",
  "llvm.x86.mmx.punpcklbw",
  "llvm.x86.mmx.punpckldq",
  "llvm.x86.mmx.punpcklwd",
  "llvm.x86.mmx.pxor",
  // miscellaneous
  "llvm.x86.pclmulqdq",
  "llvm.x86.rdfsbase.32", // nomask
  "llvm.x86.rdfsbase.64", // nomask
  "llvm.x86.rdgsbase.32", // nomask
  "llvm.x86.rdgsbase.64", // nomask
  "llvm.x86.rdrand.16", // nomask
  "llvm.x86.rdrand.32", // nomask
  "llvm.x86.rdrand.64", // nomask
  // sse2
  "llvm.x86.sse2.add.sd",//nomask
  "llvm.x86.sse2.clflush",
  "llvm.x86.sse2.cmp.pd",//nomask
  "llvm.x86.sse2.cmp.sd",//nomask
  "llvm.x86.sse2.comieq.sd",//nomask
  "llvm.x86.sse2.comige.sd",//nomask
  "llvm.x86.sse2.comigt.sd",//nomask
  "llvm.x86.sse2.comile.sd",//nomask
  "llvm.x86.sse2.comilt.sd",//nomask
  "llvm.x86.sse2.comineq.sd",//nomask
  "llvm.x86.sse2.cvtdq2pd", // nomask
  "llvm.x86.sse2.cvtdq2ps", // nomask
  "llvm.x86.sse2.cvtpd2dq", // nomask
  "llvm.x86.sse2.cvtpd2ps", // nomask
  "llvm.x86.sse2.cvtps2dq", // nomask
  "llvm.x86.sse2.cvtps2pd", // nomask
  "llvm.x86.sse2.cvtsd2si", // nomask
  "llvm.x86.sse2.cvtsd2si64", // nomask
  "llvm.x86.sse2.cvtsd2ss", // nomask
  "llvm.x86.sse2.cvtsi2sd", // nomask
  "llvm.x86.sse2.cvtsi642sd", // nomask
  "llvm.x86.sse2.cvtss2sd", // nomask
  "llvm.x86.sse2.cvttpd2dq", // nomask
  "llvm.x86.sse2.cvttps2dq", // nomask
  "llvm.x86.sse2.cvttsd2si", // nomask
  "llvm.x86.sse2.cvttsd2si64", // nomask
  "llvm.x86.sse2.div.sd",//nomask
  "llvm.x86.sse2.lfence",//nomask
  "llvm.x86.sse2.max.pd",//nomask
  "llvm.x86.sse2.max.sd",//nomask
  "llvm.x86.sse2.mfence",//nomask
  "llvm.x86.sse2.min.pd",//nomask
  "llvm.x86.sse2.min.sd",//nomask
  "llvm.x86.sse2.movmsk.pd",//nomask - mask manipulation
  "llvm.x86.sse2.mul.sd",//nomask
  "llvm.x86.sse2.packssdw.128", // nomask
  "llvm.x86.sse2.packsswb.128", // nomask
  "llvm.x86.sse2.packuswb.128", // nomask
  "llvm.x86.sse2.padds.b", // nomask
  "llvm.x86.sse2.padds.w", // nomask
  "llvm.x86.sse2.paddus.b", // nomask
  "llvm.x86.sse2.paddus.w", // nomask
  "llvm.x86.sse2.pavg.b", // nomask
  "llvm.x86.sse2.pavg.w", // nomask
  "llvm.x86.sse2.pmadd.wd", // nomask
  "llvm.x86.sse2.pmaxs.w",//nomask
  "llvm.x86.sse2.pmaxu.b",//nomask
  "llvm.x86.sse2.pmins.w",//nomask
  "llvm.x86.sse2.pminu.b",//nomask
  "llvm.x86.sse2.pmovmskb.128", //nomask - mask movement
  "llvm.x86.sse2.pmulh.w",//nomask
  "llvm.x86.sse2.pmulhu.w",//nomask
  "llvm.x86.sse2.pmulu.dq",//nomask
  "llvm.x86.sse2.psad.bw",//nomask
  "llvm.x86.sse2.psll.d",//nomask
  "llvm.x86.sse2.psll.dq",//nomask
  "llvm.x86.sse2.psll.dq.bs",//nomask
  "llvm.x86.sse2.psll.q",//nomask
  "llvm.x86.sse2.psll.w",//nomask
  "llvm.x86.sse2.pslli.d",//nomask
  "llvm.x86.sse2.pslli.q",//nomask
  "llvm.x86.sse2.pslli.w",//nomask
  "llvm.x86.sse2.psra.d",//nomask
  "llvm.x86.sse2.psra.w",//nomask
  "llvm.x86.sse2.psrai.d",//nomask
  "llvm.x86.sse2.psrai.w",//nomask
  "llvm.x86.sse2.psrl.d",//nomask
  "llvm.x86.sse2.psrl.dq",//nomask
  "llvm.x86.sse2.psrl.dq.bs",//nomask
  "llvm.x86.sse2.psrl.q",//nomask
  "llvm.x86.sse2.psrl.w",//nomask
  "llvm.x86.sse2.psrli.d",//nomask
  "llvm.x86.sse2.psrli.q",//nomask
  "llvm.x86.sse2.psrli.w",//nomask
  "llvm.x86.sse2.psubs.b",//nomask
  "llvm.x86.sse2.psubs.w",//nomask
  "llvm.x86.sse2.psubus.b",//nomask
  "llvm.x86.sse2.psubus.w",//nomask
  "llvm.x86.sse2.sqrt.pd",//nomask
  "llvm.x86.sse2.sqrt.sd",//nomask
  "llvm.x86.sse2.storel.dq",//nomask
  "llvm.x86.sse2.storeu.dq",//nomask
  "llvm.x86.sse2.storeu.pd",//nomask
  "llvm.x86.sse2.sub.sd",//nomask
  "llvm.x86.sse2.ucomieq.sd",//nomask
  "llvm.x86.sse2.ucomige.sd",//nomask
  "llvm.x86.sse2.ucomigt.sd",//nomask
  "llvm.x86.sse2.ucomile.sd",//nomask
  "llvm.x86.sse2.ucomilt.sd",//nomask
  "llvm.x86.sse2.ucomineq.sd",//nomask
  // sse3
  "llvm.x86.sse3.addsub.pd",//nomask
  "llvm.x86.sse3.addsub.ps",//nomask
  "llvm.x86.sse3.hadd.pd",//nomask
  "llvm.x86.sse3.hadd.ps",//nomask
  "llvm.x86.sse3.hsub.pd",//nomask
  "llvm.x86.sse3.hsub.ps",//nomask
  "llvm.x86.sse3.ldu.dq",//nomask
  "llvm.x86.sse3.monitor",//nomask
  "llvm.x86.sse3.mwait",//nomask
  // sse4.1
  "llvm.x86.sse41.extractps",//nomask
  "llvm.x86.sse41.insertps",//nomask
  "llvm.x86.sse41.movntdqa",//nomask
  "llvm.x86.sse41.mpsadbw",//nomask
  "llvm.x86.sse41.packusdw",//nomask
  "llvm.x86.sse41.pextrb",//nomask
  "llvm.x86.sse41.pextrd",//nomask
  "llvm.x86.sse41.pextrq",//nomask
  "llvm.x86.sse41.phminposuw",//nomask
  "llvm.x86.sse41.pmaxsb",//nomask
  "llvm.x86.sse41.pmaxsd",//nomask
  "llvm.x86.sse41.pmaxud",//nomask
  "llvm.x86.sse41.pmaxuw",//nomask
  "llvm.x86.sse41.pminsb",//nomask
  "llvm.x86.sse41.pminsd",//nomask
  "llvm.x86.sse41.pminud",//nomask
  "llvm.x86.sse41.pminuw",//nomask
  "llvm.x86.sse41.pmovsxbd",//nomask
  "llvm.x86.sse41.pmovsxbq",//nomask
  "llvm.x86.sse41.pmovsxbw",//nomask
  "llvm.x86.sse41.pmovsxdq",//nomask
  "llvm.x86.sse41.pmovsxwd",//nomask
  "llvm.x86.sse41.pmovsxwq",//nomask
  "llvm.x86.sse41.pmovzxbd",//nomask
  "llvm.x86.sse41.pmovzxbq",//nomask
  "llvm.x86.sse41.pmovzxbw",//nomask
  "llvm.x86.sse41.pmovzxdq",//nomask
  "llvm.x86.sse41.pmovzxwd",//nomask
  "llvm.x86.sse41.pmovzxwq",//nomask
  "llvm.x86.sse41.pmuldq",//nomask
  "llvm.x86.sse41.ptestc", // nomask
  "llvm.x86.sse41.ptestnzc", // nomask
  "llvm.x86.sse41.ptestz", // nomask
  "llvm.x86.sse41.round.pd", // nomask
  "llvm.x86.sse41.round.ps", // nomask
  "llvm.x86.sse41.round.sd", // nomask
  "llvm.x86.sse41.round.ss", // nomask
  // sse4.2
  "llvm.x86.sse42.crc32.32.16",//nomask
  "llvm.x86.sse42.crc32.32.32",//nomask
  "llvm.x86.sse42.crc32.32.8",//nomask
  "llvm.x86.sse42.crc32.64.64",//nomask
  "llvm.x86.sse42.crc32.64.8",//nomask
  "llvm.x86.sse42.pcmpestri128",//nomask
  "llvm.x86.sse42.pcmpestria128",//nomask
  "llvm.x86.sse42.pcmpestric128",//nomask
  "llvm.x86.sse42.pcmpestrio128",//nomask
  "llvm.x86.sse42.pcmpestris128",//nomask
  "llvm.x86.sse42.pcmpestriz128",//nomask
  "llvm.x86.sse42.pcmpestrm128",//nomask
  "llvm.x86.sse42.pcmpistri128",//nomask
  "llvm.x86.sse42.pcmpistria128",//nomask
  "llvm.x86.sse42.pcmpistric128",//nomask
  "llvm.x86.sse42.pcmpistrio128",//nomask
  "llvm.x86.sse42.pcmpistris128",//nomask
  "llvm.x86.sse42.pcmpistriz128",//nomask
  "llvm.x86.sse42.pcmpistrm128",//nomask
  "llvm.x86.sse4a.extrq",//nomask
  "llvm.x86.sse4a.extrqi",//nomask
  "llvm.x86.sse4a.insertq",//nomask
  "llvm.x86.sse4a.insertqi",//nomask
  "llvm.x86.sse4a.movnt.sd",//nomask
  "llvm.x86.sse4a.movnt.ss",//nomask
  // sse
  "llvm.x86.sse.add.ss",// nomask
  "llvm.x86.sse.cmp.ps",// nomask
  "llvm.x86.sse.cmp.ss",// nomask
  "llvm.x86.sse.comieq.ss",// nomask
  "llvm.x86.sse.comige.ss",// nomask
  "llvm.x86.sse.comigt.ss",// nomask
  "llvm.x86.sse.comile.ss",// nomask
  "llvm.x86.sse.comilt.ss",// nomask
  "llvm.x86.sse.comineq.ss",// nomask
  "llvm.x86.sse.cvtpd2pi",// nomask
  "llvm.x86.sse.cvtpi2pd",// nomask
  "llvm.x86.sse.cvtpi2ps",// nomask
  "llvm.x86.sse.cvtps2pi",// nomask
  "llvm.x86.sse.cvtsi2ss",// nomask
  "llvm.x86.sse.cvtsi642ss",// nomask
  "llvm.x86.sse.cvtss2si",// nomask
  "llvm.x86.sse.cvtss2si64",// nomask
  "llvm.x86.sse.cvttpd2pi",// nomask
  "llvm.x86.sse.cvttps2pi",// nomask
  "llvm.x86.sse.cvttss2si",// nomask
  "llvm.x86.sse.cvttss2si64",// nomask
  "llvm.x86.sse.div.ss",// nomask
  "llvm.x86.sse.ldmxcsr",// nomask
  "llvm.x86.sse.max.ps",// nomask
  "llvm.x86.sse.max.ss",// nomask
  "llvm.x86.sse.min.ps",// nomask
  "llvm.x86.sse.min.ss",// nomask
  "llvm.x86.sse.movmsk.ps",// nomask
  "llvm.x86.sse.mul.ss",// nomask
  "llvm.x86.sse.rcp.ps",// nomask
  "llvm.x86.sse.rcp.ss",// nomask
  "llvm.x86.sse.rsqrt.ps",// nomask
  "llvm.x86.sse.rsqrt.ss",// nomask
  "llvm.x86.sse.sfence",// nomask
  "llvm.x86.sse.sqrt.ps",// nomask
  "llvm.x86.sse.sqrt.ss",// nomask
  "llvm.x86.sse.stmxcsr",// nomask
  "llvm.x86.sse.storeu.ps",// nomask
  "llvm.x86.sse.sub.ss",// nomask
  "llvm.x86.sse.ucomieq.ss",// nomask
  "llvm.x86.sse.ucomige.ss",// nomask
  "llvm.x86.sse.ucomigt.ss",// nomask
  "llvm.x86.sse.ucomile.ss",// nomask
  "llvm.x86.sse.ucomilt.ss",// nomask
  "llvm.x86.sse.ucomineq.ss",// nomask
  // ssse3
  "llvm.x86.ssse3.pabs.b", // nomask
  "llvm.x86.ssse3.pabs.b.128", // nomask
  "llvm.x86.ssse3.pabs.d", // nomask
  "llvm.x86.ssse3.pabs.d.128", // nomask
  "llvm.x86.ssse3.pabs.w", // nomask
  "llvm.x86.ssse3.pabs.w.128", // nomask
  "llvm.x86.ssse3.phadd.d", // nomask
  "llvm.x86.ssse3.phadd.d.128", // nomask
  "llvm.x86.ssse3.phadd.sw", // nomask
  "llvm.x86.ssse3.phadd.sw.128", // nomask
  "llvm.x86.ssse3.phadd.w", // nomask
  "llvm.x86.ssse3.phadd.w.128", // nomask
  "llvm.x86.ssse3.phsub.d", // nomask
  "llvm.x86.ssse3.phsub.d.128", // nomask
  "llvm.x86.ssse3.phsub.sw", // nomask
  "llvm.x86.ssse3.phsub.sw.128", // nomask
  "llvm.x86.ssse3.phsub.w", // nomask
  "llvm.x86.ssse3.phsub.w.128", // nomask
  "llvm.x86.ssse3.pmadd.ub.sw", // nomask
  "llvm.x86.ssse3.pmadd.ub.sw.128", // nomask
  "llvm.x86.ssse3.pmul.hr.sw", // nomask
  "llvm.x86.ssse3.pmul.hr.sw.128", // nomask
  "llvm.x86.ssse3.psign.b",// nomask
  "llvm.x86.ssse3.psign.b.128",// nomask
  "llvm.x86.ssse3.psign.d",// nomask
  "llvm.x86.ssse3.psign.d.128",// nomask
  "llvm.x86.ssse3.psign.w",// nomask
  "llvm.x86.ssse3.psign.w.128",// nomask
  "llvm.x86.vcvtph2ps.128", // nomask
  "llvm.x86.vcvtph2ps.256", // nomask
  "llvm.x86.vcvtps2ph.128", // nomask
  "llvm.x86.vcvtps2ph.256", // nomask
  "llvm.x86.wrfsbase.32", // nomask
  "llvm.x86.wrfsbase.64", // nomask
  "llvm.x86.wrgsbase.32", // nomask
  "llvm.x86.wrgsbase.64", // nomask
  "llvm.x86.xabort", // nomask
  "llvm.x86.xbegin", // nomask
  "llvm.x86.xend", // nomask
  // xop
  "llvm.x86.xop.vfrcz.pd",
  "llvm.x86.xop.vfrcz.pd.256",
  "llvm.x86.xop.vfrcz.ps",
  "llvm.x86.xop.vfrcz.ps.256",
  "llvm.x86.xop.vfrcz.sd",
  "llvm.x86.xop.vfrcz.ss",
  "llvm.x86.xop.vpcmov",
  "llvm.x86.xop.vpcmov.256",
  "llvm.x86.xop.vpcomb",
  "llvm.x86.xop.vpcomd",
  "llvm.x86.xop.vpcomq",
  "llvm.x86.xop.vpcomub",
  "llvm.x86.xop.vpcomud",
  "llvm.x86.xop.vpcomuq",
  "llvm.x86.xop.vpcomuw",
  "llvm.x86.xop.vpcomw",
  "llvm.x86.xop.vpermil2pd",
  "llvm.x86.xop.vpermil2pd.256",
  "llvm.x86.xop.vpermil2ps",
  "llvm.x86.xop.vpermil2ps.256",
  "llvm.x86.xop.vphaddbd",
  "llvm.x86.xop.vphaddbq",
  "llvm.x86.xop.vphaddbw",
  "llvm.x86.xop.vphadddq",
  "llvm.x86.xop.vphaddubd",
  "llvm.x86.xop.vphaddubq",
  "llvm.x86.xop.vphaddubw",
  "llvm.x86.xop.vphaddudq",
  "llvm.x86.xop.vphadduwd",
  "llvm.x86.xop.vphadduwq",
  "llvm.x86.xop.vphaddwd",
  "llvm.x86.xop.vphaddwq",
  "llvm.x86.xop.vphsubbw",
  "llvm.x86.xop.vphsubdq",
  "llvm.x86.xop.vphsubwd",
  "llvm.x86.xop.vpmacsdd",
  "llvm.x86.xop.vpmacsdqh",
  "llvm.x86.xop.vpmacsdql",
  "llvm.x86.xop.vpmacssdd",
  "llvm.x86.xop.vpmacssdqh",
  "llvm.x86.xop.vpmacssdql",
  "llvm.x86.xop.vpmacsswd",
  "llvm.x86.xop.vpmacssww",
  "llvm.x86.xop.vpmacswd",
  "llvm.x86.xop.vpmacsww",
  "llvm.x86.xop.vpmadcsswd",
  "llvm.x86.xop.vpmadcswd",
  "llvm.x86.xop.vpperm",
  "llvm.x86.xop.vprotb",
  "llvm.x86.xop.vprotbi",
  "llvm.x86.xop.vprotd",
  "llvm.x86.xop.vprotdi",
  "llvm.x86.xop.vprotq",
  "llvm.x86.xop.vprotqi",
  "llvm.x86.xop.vprotw",
  "llvm.x86.xop.vprotwi",
  "llvm.x86.xop.vpshab",
  "llvm.x86.xop.vpshad",
  "llvm.x86.xop.vpshaq",
  "llvm.x86.xop.vpshaw",
  "llvm.x86.xop.vpshlb",
  "llvm.x86.xop.vpshld",
  "llvm.x86.xop.vpshlq",
  "llvm.x86.xop.vpshlw",
  "" // delimiter
};
