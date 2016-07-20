from setuptools import setup, find_packages, Extension

# 'make build' will build icmpecho wheel

setup(
    name = "icmpecho",
    version = "1.0.1-dev",
    packages=find_packages(),
    install_requires = [
        'setuptools',
    ],
    include_package_data=True,
    ext_modules=[Extension('icmpecho._network', sources=['icmpecho/_network.c'])],
    # metadata for upload to PyPI
    author = "Zenoss",
    author_email = "support@zenoss.com",
    description='Packet encode/decode of ICMPv4/6 echo and reply.',
    url = "http://www.zenoss.com/",
    zip_safe=False
)

