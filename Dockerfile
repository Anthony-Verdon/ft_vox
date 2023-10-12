FROM alpine:3.17
WORKDIR /ft_vox
RUN apk update && apk upgrade
RUN apk add \
    make \
    g++
CMD ["yes"]