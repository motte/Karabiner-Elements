all:
	@echo "Read a document at the following URL to build a distributable package."
	@echo "https://github.com/pqrs-org/Karabiner-Elements/#how-to-build"

package: clean
	./make-package.sh

notarize:
	./scripts/notarize-app.sh

staple:
	xcrun stapler staple *.dmg

build:
	$(MAKE) -C pkginfo
	$(MAKE) -C src

clean:
	$(MAKE) -C pkginfo clean
	$(MAKE) -C src clean
	rm -rf pkgroot
	rm -f *.dmg

gitclean:
	git clean -f -x -d

ibtool-upgrade:
	find * -name '*.xib' | while read f; do xcrun ibtool --upgrade "$$f"; done
