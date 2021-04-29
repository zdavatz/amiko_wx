CURRENT_VERSION=$(cat CMakeLists.txt | grep VERSION | head -n 2 | tail -n +2 | grep -Eo '[0-9\.]+')
SHOULD_DEPLOY="no"
if git rev-parse "$CURRENT_VERSION" >/dev/null 2>&1; then
  # This commit refers to a new version
  if [ $(git rev-parse "$CURRENT_VERSION") == $(git rev-parse HEAD) ] ; then
    SHOULD_DEPLOY="yes"
  fi
else
  SHOULD_DEPLOY="yes"
fi

if [ $SHOULD_DEPLOY == "yes" ] ; then
  echo $CURRENT_VERSION
fi