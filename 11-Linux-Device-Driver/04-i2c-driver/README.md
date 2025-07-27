# I2C Driver

## DTSI and DTS files
```
Device tree: Ngôn ngữ mô tả phần cứng. Mỗi node trong device tree đại diện cho một thiết bị hoặc một ngoại vi. Để driver của 1 device tìm được device tree thì nó dùng compatible.
Các file mô tả device tree có 02 định dạng: DTSI và DTS.
```

```
DTSI: Include SoC information (Hardware, Interrupt).
```

```
DTS: Include board information.
```

## Linux macro container_of
```
container_of -> get address of container structure