import sys
from gen_trait.gen_trait import main

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: python -m gen_trait <file>  (.json or .trait)")
        sys.exit(1)
    main(sys.argv[1])
