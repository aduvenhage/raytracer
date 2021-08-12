import logging
import time
import click

# https://github.com/aduvenhage/docker-machine-api
from docker_machine_api.cl_api import DockerMachine


def start_render_machine(token, scenario):
    # create new docker machine
    dm = DockerMachine(name='raytracer',
                       cwd='../',
                       config={
                            'driver': 'digitalocean',
                            'digitalocean-size': 'gd-40vcpu-160gb',
                            'digitalocean-image': 'ubuntu-18-04-x64', 
                            'digitalocean-access-token': token,
                            'engine-install-url': 'https://releases.rancher.com/install-docker/19.03.9.sh'
                       },
                       user_env={
                           'SCENARIO': scenario,
                           'OUTPUT': 'raytraced_frame.jpeg',
                           'VOLUME': '/root/output/'
                       })

    dm.tskRunServices(timeout=60*60)
    dm.tskSecureCopyFromMachine("/root/output/raytraced_frame.jpeg", "raytraced.jpeg")
    dm.tskStopMachine()
    dm.tskKillMachine()
    dm.tskRemoveMachine()
    return dm


@click.command('runner')
@click.argument('token')
def runner(token):
    logging.basicConfig(level=20)
    logger = logging.getLogger(__name__)

    dm = start_render_machine(token, 'scene0')

    # wait for rendering to complete
    idle = False
    while dm.busy() or not idle:
        idle = True
        try:
            text = dm._stdout_queue.get(block=False)
            logger.info(text)
            idle = False

        except Exception:
            pass

        try:
            text = dm._stderr_queue.get(block=False)
            logger.error(text)
            idle = False

        except Exception:
            pass

        if idle:
            time.sleep(0.2)


if __name__ == "__main__":
    runner()
