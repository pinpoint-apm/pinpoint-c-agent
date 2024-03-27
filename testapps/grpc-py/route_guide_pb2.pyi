from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class Point(_message.Message):
    __slots__ = ("latitude", "longitude")
    LATITUDE_FIELD_NUMBER: _ClassVar[int]
    LONGITUDE_FIELD_NUMBER: _ClassVar[int]
    latitude: int
    longitude: int
    def __init__(self, latitude: _Optional[int] = ..., longitude: _Optional[int] = ...) -> None: ...

class Rectangle(_message.Message):
    __slots__ = ("lo", "hi")
    LO_FIELD_NUMBER: _ClassVar[int]
    HI_FIELD_NUMBER: _ClassVar[int]
    lo: Point
    hi: Point
    def __init__(self, lo: _Optional[_Union[Point, _Mapping]] = ..., hi: _Optional[_Union[Point, _Mapping]] = ...) -> None: ...

class Feature(_message.Message):
    __slots__ = ("name", "location")
    NAME_FIELD_NUMBER: _ClassVar[int]
    LOCATION_FIELD_NUMBER: _ClassVar[int]
    name: str
    location: Point
    def __init__(self, name: _Optional[str] = ..., location: _Optional[_Union[Point, _Mapping]] = ...) -> None: ...

class RouteNote(_message.Message):
    __slots__ = ("location", "message")
    LOCATION_FIELD_NUMBER: _ClassVar[int]
    MESSAGE_FIELD_NUMBER: _ClassVar[int]
    location: Point
    message: str
    def __init__(self, location: _Optional[_Union[Point, _Mapping]] = ..., message: _Optional[str] = ...) -> None: ...

class RouteSummary(_message.Message):
    __slots__ = ("point_count", "feature_count", "distance", "elapsed_time")
    POINT_COUNT_FIELD_NUMBER: _ClassVar[int]
    FEATURE_COUNT_FIELD_NUMBER: _ClassVar[int]
    DISTANCE_FIELD_NUMBER: _ClassVar[int]
    ELAPSED_TIME_FIELD_NUMBER: _ClassVar[int]
    point_count: int
    feature_count: int
    distance: int
    elapsed_time: int
    def __init__(self, point_count: _Optional[int] = ..., feature_count: _Optional[int] = ..., distance: _Optional[int] = ..., elapsed_time: _Optional[int] = ...) -> None: ...
