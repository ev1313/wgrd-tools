### NDF File

- ndf_id: UUID PRIMARY
- vfs_path: string
- dat_path: string
- fs_path: string

### NDF Object

- object_id: UUID PRIMARY
- ndf_id: UUID NOT NULL
- object_name: string
- class_name: string
- export_path: string
- is_top_object: bool

### NDF Property

- property_id: UUID PRIMARY
- property_name: string
- property_idx: int -> if -1 the property is part of a list or map and will be ignored when iterating "all" properties of an object.
- object_id: UUID -> parent object NOT NULL
- parent: UUID -> references another property (for lists/maps/pairs), maybe null
- position: int -> set only if parent is not null
- type: int / enum -> property type
- value: UUID -> references in one of the simple type tables containing the actual values

### Simple Types, one table for each

- id: UUID PRIMARY
- value: simple type (int usually or bool or string)

### Lists / Maps / Pairs

maybe lists should not be in this and only values are stored in simple types,
lists are only properties with the fitting type, no value reference and instead only
a reference to the list of values

This would mean for maps, that all even properties are keys and all odd properties are values

pair would just have position 0 and 1

This technically also allows for lists within lists?
