FROM devkitpro/devkitarm:20190212

RUN apt-get update && \
    apt-get install -y build-essential doxygen graphviz && \
    apt-get clean

CMD /bin/bash