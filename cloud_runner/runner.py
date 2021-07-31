import logging
import time
import click

from docker_machine_api.cl_api import DockerMachine


def start_render_machine(token):
    # create new docker machine
    dm = DockerMachine(name='raytracer',
                       cwd='../',
                       config={
                            'driver': 'digitalocean', 
                            'digitalocean-image': 'ubuntu-18-04-x64', 
                            'digitalocean-access-token': token,
                            'engine-install-url': 'https://releases.rancher.com/install-docker/19.03.9.sh'
                       })

    dm.tskStartServices()
    return dm


def end_render_machine(dm):
    dm.tskStopMachine()
    dm.tskKillMachine()
    dm.tskRemoveMachine()
    dm.wait()


@click.command('runner')
@click.argument('token')
def runner(token):
    logging.basicConfig(level=20)
    logger = logging.getLogger(__name__)

    dm = start_render_machine(token)

    # wait for rendering to complete
    while True:
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
            time.sleep(0.5)


if __name__ == "__main__":
    runner()