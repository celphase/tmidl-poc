Set-Location src
try {
    & packcc -o generated tmidl.peg
}
finally {
    Set-Location ..
}
