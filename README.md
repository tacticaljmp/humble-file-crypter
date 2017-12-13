## Credits

This project is based on [m0n0ph1's Basic File Crypter](https://github.com/m0n0ph1/Basic-File-Crypter), from which I reused the whole process hollowing magic and some other stuff. So the major credit goes to him/her.
As I reordered things and tried to introduce some new features, I decided to create my own, separate project for all this.

## What this is about

This project basically enables you to spawn multiple executables and hopefully stay under AV radar. The *payloadConverter* takes the executables (payloads) you give to it and packs them all together into a C header file. This header file is used to feed the data into the actual executable package made by the *crypter*. During conversion, each payload is XORed
with a randomly generated key byte to avoid signature based detection. If you start the packed executable package, it decrypts the payloads and instanciates each of them in a newly created process by using process hollowing.

## How to actually do that

For convenient use, this project requires MinGW. You can then use the provided build scripts and proceed like:

* Define the paths to the executables you want to deploy in payloads.txt. For convenience, you may also specify command line parameters for your payloads, which will be taken into account when being deployed.

* Build everything using `completeBuild.bat`.

After that, you can stick to using `buildCrypter.bat` for building your executable packages, which skips rebuilding the *payloadConverter*.
