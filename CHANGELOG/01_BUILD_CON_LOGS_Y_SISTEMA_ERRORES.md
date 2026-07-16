

1. Build creando log de errores
```
make -j$(nproc) V=s 2>&1 | tee build.log | grep -i "error\|failed"

```


2. Si hay fallos, tira estos comandos directos al build.log
```bash
# Busca errores en tu build.log
grep -i "error" build.log | grep -v "warning" | head -50

# Busca paquetes que fallaron
grep "failed to build" build.log

# Busca el primer error grave
grep -B 5 "Error 1" build.log | head -20

# Busca líneas con "undefined reference" o "fatal error"
grep -E "(undefined reference|fatal error|missing:|No such file)" build.log | head -20
```

3. Haz las acciones correctivas. Si tocas el .conf, entonce:
```bash
make defconfig
```
