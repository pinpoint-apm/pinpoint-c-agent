package agent

type PARAMS_TYPE map[string]int32

var _id int32 = 1

type I_SendMeta interface {
	SenderGrpcMetaData(name string, id int32, Type int32, params PARAMS_TYPE)
}

type MetaData struct {
	MetaDataType int32
	IDMap        PARAMS_TYPE
	Sender       I_SendMeta
}

func (meta *MetaData) ResetMeta() {
	// reset with an empty one
	meta.IDMap = make(PARAMS_TYPE)
}

func (meta *MetaData) GetId(name string, params PARAMS_TYPE) int32 {
	id, OK := meta.IDMap[name]
	if OK {
		return id
	} else {
		_id += 1
		meta.IDMap[name] = _id
		meta.registerSelf(name, _id, meta.MetaDataType, params, meta.Sender)
		return _id
	}
}

func (meta *MetaData) registerSelf(name string, id, Type int32, params PARAMS_TYPE, sender I_SendMeta) {
	go sender.SenderGrpcMetaData(name, id, Type, params)
}
