## Credits

This project is based on [m0n0ph1's Basic File Crypter](https://github.com/m0n0ph1/Basic-File-Crypter), from which I reused the whole process hollowing magic and some other stuff. So the major credit goes to him/her.
As I reordered things and tried to introduce some new features, I decided to create my own, separate project for all this.

## What this is about

This project basically enables you to spawn multiple executables on a Windows machine and hopefully stay under AV radar. The *payloadConverter* takes the executables (payloads) you give to it and packs them all together into a C header file. This header file is used to feed the data into the actual executable package made by the *crypter*. During conversion, each payload is XORed
with a randomly generated key byte to avoid signature based detection. If you start the packed executable package, it decrypts the payloads and instanciates each of them in a newly created process by using process hollowing.
You may now also pack files into your executable package and specify where to drop them on package execution. Just like the payloads, these files will be XOR-protected.

## How to actually do that

For convenient use, this project requires MinGW. You can then use the provided build scripts and proceed like:

* Define the paths to the executables you want to deploy in *payloads.txt*.
This tells the *payloadConverter* where to look for the files when bundling your executable package. If you want to have multiple executables bundled up, specify the paths one per line. For convenience, you may also specify command line parameters for your payloads, which will be taken into account when being deployed. You may also consider the example *payloads.txt* file given in the repository.  

* Modify dropfiles.txt to specify which files you want to include in your executable package and where you want to have them dropped on execution. If you don't want to deliver any files, you can leave dropfiles.txt blank. Otherwise, the syntax per line (per drop file) is *"path to input file";"path to drop destination"* (without quotes). You may also consider the example *dropfiles.txt* file given in the repository.

* Build everything using `completeBuild.bat`.

After your first complete build, you can stick to using `buildCrypter.bat` for building your executable packages, which skips rebuilding the *payloadConverter*. The final output is always *crypter.exe*, which is the bundled executable containing your payloads and (optional) dropfiles.
