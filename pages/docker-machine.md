# install
brew install docker-machine, doctl

DO API token: https://cloud.digitalocean.com/account/api/tokens
doctl auth init [token]

# list machine sizes (slugs)
doctl compute size ls
`c-4                   8192      4        50      80.00            0.119050`
`c-32                  65536     32       400     640.00           0.952380`

# docker-machine
- create VM (ubuntu 18.04 LTS -- Digital Ocean):
  ```
  docker-machine create --driver digitalocean --digitalocean-image ubuntu-18-04-x64 --digitalocean-region sfo2 --digitalocean-size c-4 --digitalocean-access-token=... raytracer
  ```
- activate docker env: `eval $(docker-machine env raytracer) .`
- deactivate docker env: `eval $(docker-machine env -u)`
- ssh into remote machine: `docker-machine ssh raytracer`
- list machines: `docker-machine ls`
- remove machines: `docker-machine rm raytracer`
- provision a system: docker-machine (create --> ssh --> docker-compose up)

- NOTE: Docker containers may not use volumes/shares/mounts. All shared data must be copied from Dockerfiles
- NOTE: Digital Ocean VMs have to be in the same data center (in this case sfo2) as their floating IPs

