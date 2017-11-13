#ifndef SCTRSTATES_H
#define SCTRSTATES_H

enum class SimCenterViewState { visible,
                                editable,
                                hidden };

enum class SimCenterDataType { integer,
                               floatingpoint,
                               boolean,
                               string,
                               selection,
                               file,
                               tensor2D,
                               tensor3D,
                               vector2D,
                               vector3D,
                               unknown};

#endif // SCTRSTATES_H
