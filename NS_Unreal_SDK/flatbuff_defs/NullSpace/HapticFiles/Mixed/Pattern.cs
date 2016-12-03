// automatically generated by the FlatBuffers compiler, do not modify

namespace NullSpace.HapticFiles.Mixed
{

using System;
using FlatBuffers;

public struct Pattern : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static Pattern GetRootAsPattern(ByteBuffer _bb) { return GetRootAsPattern(_bb, new Pattern()); }
  public static Pattern GetRootAsPattern(ByteBuffer _bb, Pattern obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p.bb_pos = _i; __p.bb = _bb; }
  public Pattern __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public NullSpace.HapticFiles.Mixed.HapticFrame? Items(int j) { int o = __p.__offset(4); return o != 0 ? (NullSpace.HapticFiles.Mixed.HapticFrame?)(new NullSpace.HapticFiles.Mixed.HapticFrame()).__assign(__p.__indirect(__p.__vector(o) + j * 4), __p.bb) : null; }
  public int ItemsLength { get { int o = __p.__offset(4); return o != 0 ? __p.__vector_len(o) : 0; } }

  public static Offset<Pattern> CreatePattern(FlatBufferBuilder builder,
      VectorOffset itemsOffset = default(VectorOffset)) {
    builder.StartObject(1);
    Pattern.AddItems(builder, itemsOffset);
    return Pattern.EndPattern(builder);
  }

  public static void StartPattern(FlatBufferBuilder builder) { builder.StartObject(1); }
  public static void AddItems(FlatBufferBuilder builder, VectorOffset itemsOffset) { builder.AddOffset(0, itemsOffset.Value, 0); }
  public static VectorOffset CreateItemsVector(FlatBufferBuilder builder, Offset<NullSpace.HapticFiles.Mixed.HapticFrame>[] data) { builder.StartVector(4, data.Length, 4); for (int i = data.Length - 1; i >= 0; i--) builder.AddOffset(data[i].Value); return builder.EndVector(); }
  public static void StartItemsVector(FlatBufferBuilder builder, int numElems) { builder.StartVector(4, numElems, 4); }
  public static Offset<Pattern> EndPattern(FlatBufferBuilder builder) {
    int o = builder.EndObject();
    return new Offset<Pattern>(o);
  }
  public static void FinishPatternBuffer(FlatBufferBuilder builder, Offset<Pattern> offset) { builder.Finish(offset.Value); }
};


}