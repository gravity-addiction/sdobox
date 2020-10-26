
  HOSTNAME=$(hostname -s)
  while IFS= read -r HOST; do
    if [[ ${HOST} != "${HOSTNAME}.local" ]]; then
      echo ${HOST}
    fi
  done < "/opt/sdobox/scripts/sdob/child_hosts"